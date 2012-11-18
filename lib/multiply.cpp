// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <grextras/multiply.hpp>
#include <boost/make_shared.hpp>
#include <boost/cstdint.hpp>
#include <stdexcept>
#include <complex>
#include <volk/volk.h>

using namespace grextras;

/***********************************************************************
 * Templated Multiplier class
 **********************************************************************/
template <typename type>
class MultiplyImpl : public Multiply{
public:
    MultiplyImpl(const size_t num_inputs, const size_t vlen):
        SyncBlock("GrExtras Multiply"),
        _vlen(vlen)
    {
        this->set_input_signature(gras::IOSignature(sizeof(type)));
        this->set_output_signature(gras::IOSignature(sizeof(type)));
    }

    size_t sync_work(
        const InputItems &input_items,
        const OutputItems &output_items
    );

private:
    const size_t _vlen;
};

/***********************************************************************
 * Generic Multiplier implementation
 **********************************************************************/
template <typename type>
size_t MultiplyImpl<type>::sync_work(
    const InputItems &input_items,
    const OutputItems &output_items
){

    const size_t n_nums = output_items[0].size() * _vlen;
    type *out = output_items[0].cast<type *>();
    const type *in0 = input_items[0].cast<const type *>();

    for (size_t n = 1; n < input_items.size(); n++)
    {
        const type *in = input_items[n].cast<const type *>();
        for (size_t i = 0; i < n_nums; i++)
        {
            out[i] = in0[i] * in[i];
        }
        in0 = out; //for next input, we do output *= input
    }

    return output_items[0].size();
}

/***********************************************************************
 * Multiplier implementation with complex complex float32 - calls volk
 **********************************************************************/
template <>
size_t MultiplyImpl<std::complex<float> >::sync_work(
    const InputItems &input_items,
    const OutputItems &output_items
){
    const size_t n_nums = output_items[0].size() * _vlen;
    std::complex<float> *out = output_items[0].cast<std::complex<float> *>();
    const std::complex<float> *in0 = input_items[0].cast<const std::complex<float> *>();

    for (size_t n = 1; n < input_items.size(); n++)
    {
        const std::complex<float> *in = input_items[n].cast<const std::complex<float> *>();
        volk_32fc_x2_multiply_32fc(out, in0, in, n_nums);
        in0 = out; //for next input, we do output *= input
    }
    return output_items[0].size();
}

/***********************************************************************
 * Multiplier implementation with float32 - calls volk
 **********************************************************************/
template <>
size_t MultiplyImpl<float>::sync_work(
    const InputItems &input_items,
    const OutputItems &output_items
){
    const size_t n_nums = output_items[0].size() * _vlen;
    float *out = output_items[0].cast<float *>();
    const float *in0 = input_items[0].cast<const float *>();

    for (size_t n = 1; n < input_items.size(); n++)
    {
        const float *in = input_items[n].cast<const float *>();
        volk_32f_x2_multiply_32f(out, in0, in, n_nums);
        in0 = out; //for next input, we do output *= input
    }
    return output_items[0].size();
}

/***********************************************************************
 * factory function
 **********************************************************************/
Multiply::sptr Multiply::make_fc32_fc32(const size_t num_inputs, const size_t vlen)
{
    return sptr(new MultiplyImpl<std::complex<float> >(num_inputs, vlen));
}

Multiply::sptr Multiply::make_sc32_sc32(const size_t num_inputs, const size_t vlen)
{
    return sptr(new MultiplyImpl<std::complex<boost::int32_t> >(num_inputs, vlen));
}

Multiply::sptr Multiply::make_sc16_sc16(const size_t num_inputs, const size_t vlen)
{
    return sptr(new MultiplyImpl<std::complex<boost::int16_t> >(num_inputs, vlen));
}

Multiply::sptr Multiply::make_sc8_sc8(const size_t num_inputs, const size_t vlen)
{
    return sptr(new MultiplyImpl<std::complex<boost::int8_t> >(num_inputs, vlen));
}

Multiply::sptr Multiply::make_f32_f32(const size_t num_inputs, const size_t vlen)
{
    return sptr(new MultiplyImpl<float>(num_inputs, vlen));
}

Multiply::sptr Multiply::make_s32_s32(const size_t num_inputs, const size_t vlen)
{
    return sptr(new MultiplyImpl<boost::int32_t>(num_inputs, vlen));
}

Multiply::sptr Multiply::make_s16_s16(const size_t num_inputs, const size_t vlen)
{
    return sptr(new MultiplyImpl<boost::int16_t>(num_inputs, vlen));
}

Multiply::sptr Multiply::make_s8_s8(const size_t num_inputs, const size_t vlen)
{
    return sptr(new MultiplyImpl<boost::int8_t>(num_inputs, vlen));
}
