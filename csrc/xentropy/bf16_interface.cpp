#include <torch/extension.h>

// CUDA forward declarations

std::vector<at::Tensor> bf16_softmax_xentropy_cuda(
    const at::Tensor &input,
    const at::Tensor &labels,
    const bool bf16_to_float);

at::Tensor bf16_softmax_xentropy_backward_cuda(
    const at::Tensor &grad_loss,
    const at::Tensor &logits,
    const at::Tensor &max_log_sum_exp,
    const at::Tensor &labels);

// C++ interface

#define CHECK_CUDA(x) AT_ASSERTM(x.type().is_cuda(), #x " must be a CUDA tensor")
#define CHECK_CONTIGUOUS(x) AT_ASSERTM(x.is_contiguous(), #x " must be contiguous")
#define CHECK_INPUT(x) CHECK_CUDA(x); CHECK_CONTIGUOUS(x)

std::vector<at::Tensor> bf16_softmax_xentropy_forward(
    const at::Tensor &input,
    const at::Tensor &labels,
    const bool bf16_to_float) {
    CHECK_CUDA(input);
    CHECK_INPUT(labels);

    return bf16_softmax_xentropy_cuda(input, labels, bf16_to_float);
}

at::Tensor bf16_softmax_xentropy_backward(
    const at::Tensor &grad_loss,
    const at::Tensor &logits,
    const at::Tensor &max_log_sum_exp,
    const at::Tensor &labels)  {
    CHECK_CUDA(grad_loss);
    CHECK_CUDA(logits);
    CHECK_INPUT(max_log_sum_exp);
    CHECK_INPUT(labels);

    return bf16_softmax_xentropy_backward_cuda(grad_loss, logits, max_log_sum_exp, labels);
}

PYBIND11_MODULE(TORCH_EXTENSION_NAME, m) {
    m.def("forward", &bf16_softmax_xentropy_forward, "Softmax cross entropy loss forward (CUDA)");
    m.def("backward", &bf16_softmax_xentropy_backward, "Softmax cross entropy loss backward (CUDA)");
}
