/* Copyright (c) 2018 Anakin Authors, Inc. All Rights Reserved.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#include "framework/operators/fusion_ops/conv_relu_pool.h"

namespace anakin {

namespace ops {

#define INSTANCE_CONVRELUPOOLING(Ttype, Ptype) \
template<> \
void ConvReluPool<Ttype, Ptype>::operator()(\
    OpContext<Ttype>& ctx,\
    const std::vector<Tensor4dPtr<Ttype> >& ins,\
    std::vector<Tensor4dPtr<Ttype> >& outs) {\
    auto* impl = static_cast<ConvReluPoolHelper<Ttype, Ptype>*>(this->_helper);\
    auto& param = static_cast<ConvReluPoolHelper<Ttype, Ptype>*>(this->_helper)->_param_conv_relu_pooling;\
    SABER_CHECK(impl->_funcs_conv_relu_pooling(ins, outs, param, ctx));\
}

/// set helper
template<typename Ttype, Precision Ptype>
ConvReluPoolHelper<Ttype, Ptype>::~ConvReluPoolHelper() {}

template<typename Ttype, Precision Ptype>
Status ConvReluPoolHelper<Ttype, Ptype>::InitParam() {
    DLOG(WARNING) << "Parsing ConvReluPool op parameter.";
    saber::ConvParam<Ttype> conv_param_temp;
    PoolingParam<Ttype> pooling_param_temp;

    // get conv param
    auto group = GET_PARAMETER(int, group);
    auto bias_term = GET_PARAMETER(bool, bias_term);
    auto padding = GET_PARAMETER(PTuple<int>, padding);
    auto strides = GET_PARAMETER(PTuple<int>, strides);
    auto dilation_rate = GET_PARAMETER(PTuple<int>, dilation_rate);
    auto filter_num = GET_PARAMETER(int, filter_num);
    auto kernel_size = GET_PARAMETER(PTuple<int>, kernel_size);
    auto axis = GET_PARAMETER(int, axis);

	using pblock_type = PBlock<Ttype>;
    auto weights = GET_PARAMETER(pblock_type, weight_1);
    // resize weights scale
    auto& w = weights.h_tensor();
    if (w.get_scale().size() == 1){
        float scale_tmp = w.get_scale()[0];
        std::vector<float> w_scale(filter_num, scale_tmp);
        w.set_scale(w_scale);
    }
    // get relu param
    auto alpha = GET_PARAMETER(float, relu_0_alpha);
    ActivationParam<Ttype> active_param(Active_relu, alpha); // Temp

    // get pooling param
    auto global_pooling = GET_PARAMETER(bool, pooling_0_global_pooling);
    auto pool_padding = GET_PARAMETER(PTuple<int>, pooling_0_padding);
    auto pool_strides = GET_PARAMETER(PTuple<int>, pooling_0_strides);
    auto pool_size = GET_PARAMETER(PTuple<int>, pooling_0_pool_size);
    auto pool_method = GET_PARAMETER(std::string, pooling_0_method);
    auto cmp_out_shape_floor_as_conv = GET_PARAMETER(bool, pooling_0_cmp_out_shape_floor_as_conv);
    if (pool_method == "MAX") {
        PoolingParam<Ttype> pooling_param(pool_size[0], pool_size[1],
                pool_padding[0], pool_padding[1], pool_strides[0], pool_strides[1],
                Pooling_max, global_pooling, cmp_out_shape_floor_as_conv);
        pooling_param_temp = pooling_param;
    } else if (pool_method == "AVG") {
        PoolingParam<Ttype> pooling_param(pool_size[0], pool_size[1],
                pool_padding[0], pool_padding[1], pool_strides[0], pool_strides[1],
                Pooling_average_include_padding, global_pooling,
                cmp_out_shape_floor_as_conv);
        pooling_param_temp = pooling_param;
    } else {
        LOG(FATAL) << " ConvReluPool fusion op doesn't support : " << pool_method << " pooling.";
    }

    if (bias_term) {
        auto bias = GET_PARAMETER(pblock_type, weight_2);
        saber::ConvParam<Ttype> conv_param(group, padding[0], padding[1],
                strides[0], strides[1], dilation_rate[0], dilation_rate[1],
                &(weights.d_tensor()), &(bias.d_tensor()),
                active_param);
        conv_param_temp = conv_param;
    } else {
        Tensor4d<Ttype>* bias = new Tensor4d<Ttype>();
        saber::ConvParam<Ttype> conv_param(group, padding[0], padding[1],
                strides[0], strides[1], dilation_rate[0], dilation_rate[1],
                &(weights.d_tensor()), bias, active_param);
        conv_param_temp = conv_param;
    }

    ConvPoolingParam<Ttype> conv_act_pooling_param(conv_param_temp, pooling_param_temp);
    _param_conv_relu_pooling = conv_act_pooling_param;

    return Status::OK();
}

template<typename Ttype, Precision Ptype>
Status ConvReluPoolHelper<Ttype, Ptype>::Init(OpContext<Ttype> &ctx,
        const std::vector<Tensor4dPtr<Ttype> >& ins,
        std::vector<Tensor4dPtr<Ttype> >& outs) {

    auto group = GET_PARAMETER(int, group);
    auto strides = GET_PARAMETER(PTuple<int>, strides);
    auto weights = GET_PARAMETER(PBlock<Ttype>, weight_1);
    auto bias_term = GET_PARAMETER(bool, bias_term);

#ifdef AMD_GPU
    saber::ImplEnum impl_e = SABER_IMPL;
#else
    saber::ImplEnum impl_e = SABER_IMPL;
    if (std::is_same<Ttype, X86>::value || std::is_same<Ttype, ARM>::value) {
        impl_e = SABER_IMPL;
    }
#endif

    _funcs_conv_relu_pooling.init(ins, outs, _param_conv_relu_pooling, SPECIFY,
                                  impl_e, ctx);

    // check if weights have been transposed
    auto is_weights_transed = CHECK_PARAMETER(is_weights_transed);
    if (!is_weights_transed) {
        SET_PARAMETER(is_weights_transed, true, bool);
        if (bias_term) {
            auto bias = GET_PARAMETER(PBlock<Ttype>, weight_2);
            graph::GraphGlobalMem<Ttype>::Global().template apply<Level_1>(
                    std::bind(&ConvPooling<Ttype,
                                      PrecisionWrapper<Ptype>::saber_type>::trans_weights,
                              &_funcs_conv_relu_pooling, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10),
                    weights.d_tensor(), bias.d_tensor(), _param_conv_relu_pooling.conv_param.pad_h, _param_conv_relu_pooling.conv_param.pad_w, _param_conv_relu_pooling.conv_param.dilation_h, _param_conv_relu_pooling.conv_param.dilation_w,
                    strides[0], strides[1], group, impl_e);
            bias.map_to_host();
        } else {
            PBlock<Ttype> bias_empty;
            graph::GraphGlobalMem<Ttype>::Global().template apply<Level_1>(
                    std::bind(&ConvPooling<Ttype,
                                      PrecisionWrapper<Ptype>::saber_type>::trans_weights,
                              &_funcs_conv_relu_pooling, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10),
                    weights.d_tensor(), bias_empty.d_tensor(), _param_conv_relu_pooling.conv_param.pad_h, _param_conv_relu_pooling.conv_param.pad_w, _param_conv_relu_pooling.conv_param.dilation_h, _param_conv_relu_pooling.conv_param.dilation_w,
                    strides[0], strides[1], group, impl_e);
        }
        weights.map_to_host();

    } else {
        PBlock<Ttype> weight_empty;
        PBlock<Ttype> bias_empty;
        graph::GraphGlobalMem<Ttype>::Global().template apply<Level_1>(
                std::bind(&ConvPooling<Ttype, PrecisionWrapper<Ptype>::saber_type>::trans_weights,
                          &_funcs_conv_relu_pooling, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10),
                weight_empty.d_tensor(), bias_empty.d_tensor(), _param_conv_relu_pooling.conv_param.pad_h, _param_conv_relu_pooling.conv_param.pad_w, _param_conv_relu_pooling.conv_param.dilation_h, _param_conv_relu_pooling.conv_param.dilation_w,
                strides[0], strides[1], group, impl_e);
    }
    return Status::OK();
}

template<typename Ttype, Precision Ptype>
Status ConvReluPoolHelper<Ttype, Ptype>::InferShape(const std::vector<Tensor4dPtr<Ttype> >& ins,
        std::vector<Tensor4dPtr<Ttype> >& outs) {
   SABER_CHECK(_funcs_conv_relu_pooling.compute_output_shape(ins, outs, _param_conv_relu_pooling));
   return Status::OK();
}

#ifdef USE_CUDA
INSTANCE_CONVRELUPOOLING(NV, Precision::FP32);
INSTANCE_CONVRELUPOOLING(NV, Precision::INT8);
ANAKIN_REGISTER_OP_HELPER(ConvReluPool, ConvReluPoolHelper, NV, Precision::FP32);
ANAKIN_REGISTER_OP_HELPER(ConvReluPool, ConvReluPoolHelper, NV, Precision::INT8);
#endif

#ifdef USE_X86_PLACE
INSTANCE_CONVRELUPOOLING(X86, Precision::FP32);
ANAKIN_REGISTER_OP_HELPER(ConvReluPool, ConvReluPoolHelper, X86, Precision::FP32);
INSTANCE_CONVRELUPOOLING(X86, Precision::INT8);
ANAKIN_REGISTER_OP_HELPER(ConvReluPool, ConvReluPoolHelper, X86, Precision::INT8);
#endif

#ifdef USE_ARM_PLACE
INSTANCE_CONVRELUPOOLING(ARM, Precision::FP32);
template class ConvReluPoolHelper<ARM, Precision::FP32>;
ANAKIN_REGISTER_OP_HELPER(ConvReluPool, ConvReluPoolHelper, ARM, Precision::FP32);
#endif

#ifdef AMD_GPU
INSTANCE_CONVRELUPOOLING(AMD, Precision::FP32);
template class ConvReluPoolHelper<AMD, Precision::FP32>;
ANAKIN_REGISTER_OP_HELPER(ConvReluPool, ConvReluPoolHelper, AMD, Precision::FP32);
#endif

#if defined BUILD_LITE
INSTANCE_CONVRELUPOOLING(X86, Precision::FP32);
template class ConvReluPoolHelper<X86, Precision::FP32>;
ANAKIN_REGISTER_OP_HELPER(ConvReluPool, ConvReluPoolHelper, X86, Precision::FP32);
#endif
//! register op
ANAKIN_REGISTER_OP(ConvReluPool)
.Doc("ConvReluPool fusion operator")
#ifdef USE_CUDA
.__alias__<NV, Precision::FP32>("convolution_relu_pooling")
.__alias__<NV, Precision::INT8>("convolution_relu_pooling")
#endif
#ifdef USE_ARM_PLACE
.__alias__<ARM, Precision::FP32>("convolution_relu_pooling")
#endif
#ifdef AMD_GPU
.__alias__<AMD, Precision::FP32>("convolution_relu_pooling")
#endif

#if defined BUILD_LITE
.__alias__<X86, Precision::FP32>("convolution_batchnorm_scale_relu_pooling")
#endif
.num_in(1)
.num_out(1)
.Args<int>("group", " group of conv ")
.Args<bool>("bias_term", " whether conv weights have bias")
.Args<PTuple<int>>("padding", "padding of conv (x, y)")
.Args<PTuple<int>>("strides", "strides of conv (x)")
.Args<PTuple<int>>("dilation_rate", "dilation rate of conv (x)")
.Args<int>("filter_num", "filter(kernel) number of weights")
.Args<PTuple<int>>("kernel_size", "kernel size of kernel (x, y)")
.Args<int>("axis", "axis of conv")
.Args<bool>("pooling_0_global_pooling", " whether use pooling for all input area.")
.Args<PTuple<int>>("pooling_0_padding", " paddding of pooling ")
.Args<PTuple<int>>("pooling_0_strides", " strides of pooling ")
.Args<PTuple<int>>("pooling_0_pool_size", "pooling size of pooling")
.Args<std::string>("pooling_0_method", " pooling methods")
.Args<bool>("pooling_0_cmp_out_shape_floor_as_conv", "cmp_out_shape_floor_as_conv")
.Args<float>("relu_0_alpha", " alpha for relu");

} /* namespace ops */

} /* namespace anakin */


