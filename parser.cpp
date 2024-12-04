#include <algorithm>
#include <string.h>
#include "parser.h"
#include "exception.h"

namespace ncc
{

class Parser
{
	Parser(const std::string& code)
	: m_code(code)
	{
		// nop
	}

	void parse();

	size_t consume(char *buf, size_t sizeBuf)
	{
		size_t left = m_code.size() - m_sizeConsumed;
		size_t sizeToConsume = std::min(left, sizeBuf);
		::memcpy(buf, m_code.data() + m_sizeConsumed, sizeToConsume);
		m_sizeConsumed += sizeToConsume;

		return sizeToConsume;
	}

	typedef size_t noded_t;

	noded_t addNode(PASTNode newnode)
	{
		std::cout << "addNode: " << *newnode << std::endl;

		m_astnodes.push_back(std::move(newnode));
		return m_astnodes.size() - 1;
	}

	const PASTNode& getNode(noded_t s)
	{
		return m_astnodes[s];
	}

	std::vector<PASTNode> getNodes(const std::vector<noded_t>& nodeds)
	{
		std::vector<PASTNode> ret; ret.resize(nodeds.size());
		std::transform(nodeds.begin(), nodeds.end(), ret.begin(), std::bind(&Parser::getNode, this, std::placeholders::_1));

		return std::move(ret);
	}

	void setRootNode(noded_t s)
	{
		setRootNode(getNode(s));
	}

	void setRootNode(PASTNode node)
	{
		m_root = node;
	}

	const PASTNode& getRootNode()
	{
		return m_root;
	}

private:
	const std::string& m_code;
	size_t m_sizeConsumed = 0;

	std::vector<PASTNode> m_astnodes;
	PASTNode m_root;
};

#define YYSTYPE Parser::noded_t
#define yythis ctx->pthis
#define YY_INPUT(buf, result, max_size) \
	do {result = yythis->consume(buf, max_size);} while(0)
#define YY_CTX_LOCAL
#define YY_CTX_MEMBERS Parser* pthis;
#include "parse.impl.h"

namespace
{
	void yyfree(yycontext* ctx)
	{
		if (ctx->buf) ::free(ctx->buf);
		if (ctx->text) ::free(ctx->text);
		if (ctx->thunks) ::free(ctx->thunks);
		if (ctx->vals) ::free(ctx->vals);
	}
}

void
Parser::parse()
{
	yycontext ctx;
	::memset(&ctx, 0, sizeof(yycontext));
	ctx.pthis = this;

	int r = yyparse(&ctx);
	yyfree(&ctx);

	if (r == 0)
		throw ParseError();
}

PASTNode
parse(const std::string& code)
{
	Parser p(code); p.parse();
	return p.getRootNode();
}
}