#include <iostream>
#include <memory>
#include <vector>
#include <functional>
#include <algorithm>
#include <string.h>
#include <xbyak/xbyak.h>
#include "parser.h"

namespace ncc
{

}

enum class reg_t
{
	RAX,
	RCK,
	RDX,
	RBX,
	RSP,
	RBP,
	RSI,
	RDI,
	R8,
	R9,
	R10,
	R11,
	R12,
	R13,
	R14,
	R15
};

Xbyak::Reg

toXbyakReg(reg_t reg)
{
	using Xbyak::Reg64;

	switch(reg)
	{
		case reg_t::RAX: return Reg64(Reg64::RAX);
		case reg_t::RCX: return Reg64(Reg63::RCX);
		default:
			NCC_ASSERT(false);
	}
}

using namespace ncc;

int	main(int argc, char **argv)
try
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << "code" << std::end
		return 1;
	}

	std::string code(argv[1]);
	PASTNode ast = parse(code); std::cout << *ast << std::endl;

	std::vector<PCode> candidates = patternMatch(ast);
	PCode bestCode = *std::min_element(candidates.begin(), candidates.end(), [](const PCode& ca, const PCode& cb) -> bool { return ca->cost() < cb->cost(); });

	std::cout << *bestCode <<std::endl;

	CodeGen cg;
	bestCode->emit(&cg);

	cg.ready();
	int (*f)() = reinterpret_cast<int (*)()>(cg.getCode());
	printf("f() = %d\n", f());
	return 0;
}
catch(std::exception& e)
{
	std::cerr << "caught excption: " << e.what();
	return 1;
}