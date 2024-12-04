#ifndef _ncc_parser_h_
#define _ncc_parser_h_

#include "ast.h"

namespace ncc
{
	PASTNode parse(const std::string& code);
}

#endif