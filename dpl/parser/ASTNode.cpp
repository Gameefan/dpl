#include "ASTNode.h"
#include "../utils/ErrorReporter.h"

void ASTNode::dump(int depth, bool highlight_tokens)
{
	for (size_t i = 0; i < depth; i++)
	{
		printf("\t");
	}

	printf("%s ", node_name());
	print_custom_data();
	printf(" @ %p\n", this);
	if (highlight_tokens) {
		ErrorReporter::highlight_token(token());
	}
	for (ASTNode* child : m_children) {
		child->dump(depth + 1, highlight_tokens);
	}
}

void ASTNode::set_parent(ASTNode* parent)
{
	m_parent = parent;
}

ASTNode* ASTNode::parent()
{
	return m_parent;
}

void ASTNode::add_child(ASTNode* node)
{
	m_children.push_back(node);
	node->set_parent(this);
}

std::vector<ASTNode*> ASTNode::children()
{
	return m_children;
}
