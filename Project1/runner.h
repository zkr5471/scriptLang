#pragma once

namespace Xscript
{

	Value run_stmt(Node *node);
	Value run_expr(Node *node);

	Value run_builtin_func(Node *node);

}