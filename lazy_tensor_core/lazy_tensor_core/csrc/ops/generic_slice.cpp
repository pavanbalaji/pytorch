#include "lazy_tensor_core/csrc/ops/generic_slice.h"

#include "lazy_tensor_core/csrc/ts_backend/ts_shape_inference.h"
#include "lazy_tensor_core/csrc/ops/ltc_ops.h"

namespace torch_lazy_tensors {
namespace ir {
namespace ops {

GenericSlice::GenericSlice(const torch::lazy::Value& input,
                           c10::ArrayRef<int64_t> base_indices,
                           c10::ArrayRef<int64_t> sizes)
    : TsNode(ltc_generic_slice, {input},
             /*num_outputs=*/1, torch::lazy::MHash(base_indices, sizes)),
      base_indices_(base_indices.begin(), base_indices.end()),
      sizes_(sizes.begin(), sizes.end()) {
  SetShapeDeferred(
      [&]() { return compiler::InferShape(this); });
}

NodePtr GenericSlice::Clone(OpList operands) const {
  return torch::lazy::MakeNode<GenericSlice>(operands.at(0), base_indices_, sizes_);
}

std::string GenericSlice::ToString() const {
  std::stringstream ss;
  ss << TsNode::ToString() << ", base_indices=("
     << c10::Join(", ", base_indices_) << "), sizes=("
     << c10::Join(", ", sizes_) << ")";
  return ss.str();
}

}  // namespace ops
}  // namespace ir
}  // namespace torch_lazy_tensors
