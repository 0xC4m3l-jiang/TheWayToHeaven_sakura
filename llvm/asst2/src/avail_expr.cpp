#include "../include/dfa/framework.h"


namespace {

class Expression
{
private:
        unsigned _opcode;
        const Value * _lhs, * _rhs;
public:
    //@DONE 更具 实现的功能初始化表的值。
        Expression(const Instruction & inst)
        {
                // @TODO  得到符号，参数，因为只有两个参数，所以用到 getOpcode(0)  getopcode(1)
            _opcode = inst.getOpcode();
            _lhs = inst.getOperand(0);
            _rhs = inst.getOperand(1);
        }
        // 定义 == 。判断是否是 计算式。
        bool operator==(const Expression & Expr) const
        {
                // @TODO
            bool isEquOperand = false;
            switch (_opcode) {
                case Instruction::Add:
                case Instruction::FAdd:
                case Instruction::Mul:
                case Instruction::FMul:
                case Instruction::And:
                case Instruction::Or:
                case Instruction::Xor:
                    isEquOperand = (Expr.getLHSOperand() == _lhs && Expr.getRHSOperand() == _rhs)
                                   || (Expr.getLHSOperand() == _rhs && Expr.getRHSOperand() == _lhs);
                    break;
                default:
                    isEquOperand = Expr.getLHSOperand() == _lhs && Expr.getRHSOperand() == _rhs;
            }
            return Expr.getOpcode() == _opcode && isEquOperand;
        }

        unsigned getOpcode() const { return _opcode; }
        const Value * getLHSOperand() const { return _lhs; }
        const Value * getRHSOperand() const { return _rhs; }

        friend raw_ostream & operator<<(raw_ostream & outs, const Expression & expr);
}; 

raw_ostream & operator<<(raw_ostream & outs, const Expression & expr)
{
        outs << "[" << Instruction::getOpcodeName(expr._opcode) << " ";
        expr._lhs->printAsOperand(outs, false); outs << ", ";
        expr._rhs->printAsOperand(outs, false); outs << "]";

        return outs;
}

}  // namespace anonymous

namespace std {

// Construct a hash code for 'Expression'.
template <>
struct hash < Expression >
{
        std::size_t operator()(const Expression & expr) const
        {
                std::hash < unsigned > unsigned_hasher; std::hash < const Value * > pvalue_hasher;

                std::size_t opcode_hash = unsigned_hasher(expr.getOpcode());
                std::size_t lhs_operand_hash = pvalue_hasher((expr.getLHSOperand()));
                std::size_t rhs_operand_hash = pvalue_hasher((expr.getRHSOperand()));

                return opcode_hash ^ (lhs_operand_hash << 1) ^ (rhs_operand_hash << 1);
        }
};

}  // namespace std

namespace {

class AvailExpr final : public dfa::Framework < Expression, 
                                                dfa::Direction::Forward >
{
protected:
        virtual BitVector IC() const override
        {
                // @TODO
                // 全集
                return BitVector(_domain.size(),true);
        }

        virtual BitVector BC() const override
        {
                // @TODO
                // 空集
                return BitVector(_domain.size(),false);
        }


        // 实现并集
        virtual BitVector MeetOp(const meetop_const_range & meet_operands) const override
        {
                // @TODO
            BitVector result(_domain.size(), true);
            auto B = meet_operands.begin();
            auto E = meet_operands.end();
            for (; B != E; ++B) {
                auto term = (*B)->getTerminator();
                result &= _inst_bv_map.at(term);
            }
            return result;
        }

        // gen U (IN- kill)   到达定值方程的计算
        // ibv => 基础块
        virtual bool TransferFunc(const Instruction & inst,
                                  const BitVector & ibv,
                                  BitVector & obv) override{
                // @TODO
            BitVector bv = ibv;
            // gen
            if (isa<BinaryOperator>(inst) && _domain.find(Expression(inst)) != _domain.end())
                bv[getVN(Expression(inst))] = true;

            // in-kill
            // 在基础块中定义了的
            for (auto expr:_domain) {
                if (expr.getLHSOperand() == &inst || expr.getRHSOperand() == &inst) {
                    bv[getVN(expr)] = false;
                }
            }
            bool changed = bv != obv;
            obv = bv;
            return changed;
        }


        virtual void InitializeDomainFromInstruction(const Instruction & inst) override
        {
                if (isa < BinaryOperator > (inst))
                {
                        _domain.emplace(inst);
                }
        }
public:
        static char ID;

        AvailExpr() : dfa::Framework < domain_element_t, 
                                       direction_c > (ID) {}
        virtual ~AvailExpr() override {}
};

char AvailExpr::ID = 1; 
RegisterPass < AvailExpr > Y ("avail_expr", "Available Expression");

} // namespace anonymous
