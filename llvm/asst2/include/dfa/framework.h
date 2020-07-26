#pragma once

#include <cassert>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

#include <llvm/Pass.h>
#include <llvm/ADT/BitVector.h>
#include <llvm/ADT/PostOrderIterator.h>
#include <llvm/IR/CFG.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

namespace dfa {
/// Analysis Direction, used as Template Parameter
    enum class Direction {
        Forward, Backward
    };

/// Dataflow Analysis Framework
///
/// @tparam TDomainElement  Domain Element
/// @tparam TDirection      Direction of Analysis
    template<typename TDomainElement, Direction TDirection>
    class Framework : public FunctionPass {
/// This macro selectively enables methods depending on the analysis direction.
///
/// @param dir  Direction of Analysis
/// @param ret  Return Type
#define METHOD_ENABLE_IF_DIRECTION(dir, ret)                                    \
        template < Direction _TDirection = TDirection >                         \
        typename std::enable_if < _TDirection == dir, ret > ::type
/// This macro does typedef depending on the analysis direction. If the
/// direction in the template argument does not match the argument, the
/// resulting type is not defined.
///
/// @param type_name  Name of the Resulting Type
/// @param dir        Direction of Analysis
/// @param T          Type Definition if `TDirection == dir`
/// @comment The original definition works bad, we change it in another way.
/// @note This require c++14. For c++11 usage, change it to
///             using type_name = std::conditional<dir == TDirection, TrueTy, FalseTy>::type
//#define TYPEDEF_IF_DIRECTION(type_name, dir, T)                                 \
//        using type_name = typename std::enable_if < TDirection == dir, T > ::type
#define TYPEDEF_IF_DIRECTION(type_name, dir, TrueTy, FalseTy)                                 \
        using type_name = std::conditional_t<dir == TDirection, TrueTy, FalseTy>;
/// @todo For ∀ @c METHOD_ENABLE and @c TYPEDEF , you will have to add the
///       equivalent definition for backward analysis.
    protected:
        typedef TDomainElement domain_element_t;
        static constexpr Direction direction_c = TDirection;
        /***********************************************************************
         * Domain
         ***********************************************************************/
        std::unordered_set<TDomainElement> _domain;
        /***********************************************************************
         * Instruction-BitVector Mapping
         ***********************************************************************/
        /// Mapping from Instruction Pointer to BitVector
        std::unordered_map<const Instruction *, BitVector> _inst_bv_map;

        /// @brief Return the initial condition.
        ///
        /// @todo  Override this method in every child class.
        virtual BitVector IC() const = 0;

        /// @brief Return the boundary condition.
        ///
        /// @todo  Override this method in every child class.
        virtual BitVector BC() const = 0;

    protected:
        /// \comment As the unordered set might has different offset with your input construction order, search it when use.
        ///
        /// \param domainElement
        ///
        /// \return the numbering result of value, -1 if not found the element
        // 获得 在 domain 中的偏移
        int getVN(const TDomainElement &domainElement) const {
            auto inst_dom_iter = _domain.find(domainElement);
            if (inst_dom_iter == _domain.end()) {
                return -1;
            }
            int idx = 0;
            for (auto dom_iter = _domain.begin(); inst_dom_iter != dom_iter; ++dom_iter)
                ++idx;
            return idx;
        }

//        /// @brief value numbering all domain's elements
//        void valueNumberingTheDomain() {
//            int idx = 0;
//            auto E = _domain.end();
//            for (auto dom_iter = _domain.begin(); dom_iter != dom_iter; ++dom_iter) {
//                (*dom_iter).setVN(idx++);
//            }
//        }

    private:
        /// @brief Dump the domain under @p mask . E.g., If @c domain = {%1, %2,
        ///        %3,}, dumping it with @p mask = 001 will give {%3,}.
        void printDomainWithMask(const BitVector &mask) const {
            outs() << "{";

            assert(mask.size() == _domain.size() &&
                   "The size of mask must be equal to the size of domain.");

            unsigned mask_idx = 0;
            for (const auto &elem : _domain) {
                if (!mask[mask_idx++]) {
                    continue;
                }
                outs() << elem << ", ";
            }  // for (mask_idx ∈ [0, mask.size()))
            outs() << "}";
        }

        METHOD_ENABLE_IF_DIRECTION(Direction::Forward, void)
        printInstBV(const Instruction &inst) const {
            const BasicBlock *const pbb = inst.getParent();

            if (&inst == &(*pbb->begin())) {
                meetop_const_range meet_operands = MeetOperands(*pbb);
                // If the list of meet operands is empty, then we are at
                // the boundary, hence print the BC.
                if (meet_operands.begin() == meet_operands.end()) {
                    outs() << "BC:\t";
                    printDomainWithMask(BC());
                    outs() << "\n";
                } else {
                    outs() << "MeetOp:\t";
                    printDomainWithMask(MeetOp(meet_operands));
                    outs() << "\n";
                }
            }
            outs() << "Instruction: " << inst << "\n";
            outs() << "\t";
            printDomainWithMask(_inst_bv_map.at(&inst));
            outs() << "\n";
        }

        // FIXME , ADDED
        METHOD_ENABLE_IF_DIRECTION(Direction::Backward, void)
        printInstBV(const Instruction &inst) const {
            const BasicBlock *const pbb = inst.getParent();

            if (&inst == &(*pbb->begin())) {
                meetop_const_range meet_operands = MeetOperands(*pbb);
                // If the list of meet operands is empty, then we are at
                // the boundary, hence print the BC.
                // 对于基本快数据的打印。
                if (meet_operands.begin() == meet_operands.end()) {
                    outs() << "BC:\t";
                    printDomainWithMask(BC());
                    outs() << "\n";
                } else {
                    outs() << "MeetOp:\t";
                    printDomainWithMask(MeetOp(meet_operands));
                    outs() << "\n";
                }
            }
            outs() << "Instruction: " << inst << "\n";
            outs() << "\t";
            printDomainWithMask(_inst_bv_map.at(&inst));
            outs() << "\n";
        }

    protected:
        /// @brief Dump, ∀inst ∈ @p F, the associated bitvector.
        void printInstBVMap(const Function &F) const {
            outs() << "********************************************" << "\n";
            outs() << "* Instruction-BitVector Mapping             " << "\n";
            outs() << "********************************************" << "\n";

            for (const auto &inst : instructions(F)) {
                printInstBV(inst);
            }
        }
        /***********************************************************************
         * Meet Operator and Transfer Function
         ***********************************************************************/
        using meetop_const_range = std::conditional_t<
                Direction::Forward == TDirection, pred_const_range, succ_const_range>;
//        TYPEDEF_IF_DIRECTION(meetop_const_range,
//                             Direction::Forward,
//                             pred_const_range);
        /// @brief Return the operands for the meet operation.
        METHOD_ENABLE_IF_DIRECTION(Direction::Forward, meetop_const_range)
        MeetOperands(const BasicBlock &bb) const {
            return predecessors(&bb);
        }

        /// @DONE
        /// @brief Return the operands for the meet operation.
        METHOD_ENABLE_IF_DIRECTION(Direction::Backward, meetop_const_range)
        MeetOperands(const BasicBlock &bb) const {
            return successors(&bb);
        }

        /// @brief  Apply the meet operation to a range of @p meet_operands .
        ///
        /// @return the Resulting BitVector after the Meet Operation
        ///
        /// @todo   Override this method in every child class.
        virtual BitVector MeetOp(const meetop_const_range &meet_operands) const = 0;

        /// @brief  Apply the transfer function at instruction @p inst to the
        ///         input bitvector to get the output bitvector.
        ///
        /// @return True if @p obv has been changed, False otherwise
        ///
        /// @todo   Override this method in every child class.
        virtual bool TransferFunc(const Instruction &inst,
                                  const BitVector &ibv,
                                  BitVector &obv) = 0;
        /***********************************************************************
         * CFG Traversal
         ***********************************************************************/
    private:
//        TYPEDEF_IF_DIRECTION(bb_traversal_const_range,
//                             Direction::Forward,
//                             iterator_range<Function::const_iterator>
//        );
//        TYPEDEF_IF_DIRECTION(inst_traversal_const_range,
//                             Direction::Forward,
//                             iterator_range<BasicBlock::const_iterator>
//        );
        TYPEDEF_IF_DIRECTION(bb_traversal_const_range, Direction::Forward,
                             iterator_range<Function::const_iterator>,
                             iterator_range<SymbolTableList<BasicBlock>::const_reverse_iterator>);
        TYPEDEF_IF_DIRECTION(inst_traversal_const_range, Direction::Forward,
                             iterator_range<BasicBlock::const_iterator>,
                             iterator_range<SymbolTableList<Instruction>::const_reverse_iterator>);

        /// @brief Return the traversal order of the basic blocks.
        ///
        /// @todo  Modify the implementation (and the above typedef accordingly)
        ///        for the optimal traversal order.
        METHOD_ENABLE_IF_DIRECTION(Direction::Forward, bb_traversal_const_range)
        BBTraversalOrder(const Function &F) const {
            return make_range(F.begin(), F.end());
        }
        /// @brief Return the traversal order of the instructions.
        METHOD_ENABLE_IF_DIRECTION(Direction::Forward, inst_traversal_const_range)
        InstTraversalOrder(const BasicBlock &bb) const {
            return make_range(bb.begin(), bb.end());
        }

        METHOD_ENABLE_IF_DIRECTION(Direction::Backward, bb_traversal_const_range)
        BBTraversalOrder(const Function &F) const {
            return make_range(F.getBasicBlockList().rbegin(), F.getBasicBlockList().rend());
        }
        /// @brief Return the traversal order of the instructions.
        METHOD_ENABLE_IF_DIRECTION(Direction::Backward, inst_traversal_const_range)
        InstTraversalOrder(const BasicBlock &bb) const {
            return make_range(bb.getInstList().rbegin(), bb.getInstList().rend());
        }

    protected:
        /// @brief  Traverse through the CFG and update @c inst_bv_map .
        ///
        /// @return True if changes are made to @c inst_bv_map, False otherwise
        ///
        /// @todo   Implement this method for all the child classes.
        bool traverseCFG(const Function &func) {
            bool changed = false;
            // 获得进入的 BasicBlock。 通过返回的 BasicBlockList 的表的 degin
            auto entry = &*(BBTraversalOrder(func).begin());
            for (auto &block:BBTraversalOrder(func)) {
                BitVector ibv;
                // 获得对应的 BasicBlock
                if (&block == entry) {
                    ibv = BC();
                } else {
                    ibv = MeetOp(MeetOperands(block));
                }
                // 依次得到每个块的 intput 和 output
                for (auto &inst:InstTraversalOrder(block)) {
                    changed |= TransferFunc(inst, ibv, _inst_bv_map[&inst]);
                    ibv = _inst_bv_map[&inst];
                }
            }
            return changed;

        }

    public:
        Framework(char ID) : FunctionPass(ID) {}

        virtual ~Framework() override {}

        // We don't modify the program, so we preserve all analysis.
        virtual void getAnalysisUsage(AnalysisUsage &AU) const {
            AU.setPreservesAll();
        }

    protected:
        /// @brief Initialize the domain from each instruction.
        /// 初始化。
        /// @todo  Override this method in every child class.
        virtual void InitializeDomainFromInstruction(const Instruction &inst) = 0;

    public:
        virtual bool runOnFunction(Function &F) override final {
            // 循环 对 inst 置零。
            for (const auto &inst : instructions(F)) {
                InitializeDomainFromInstruction(inst);
            }
            // 循环将 inst 放入 _inst_bv_map
            for (const auto &inst : instructions(F)) {
                _inst_bv_map.emplace(&inst, IC());
            }
            // keep traversing until changes have been made to the
            // instruction-bv mapping
            // 循环对 Function 进行一个 操作
            while (traverseCFG(F)) {}
            // 打印出 变化后的值。
            printInstBVMap(F);

            return false;
        }

#undef METHOD_ENABLE_IF_DIRECTION
    };

}  // namespace dfa