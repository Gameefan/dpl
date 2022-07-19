#pragma once
#include <vector>
#include <cassert>
#include "../lexer/LexerToken.h"

class ASTNode {
public:
	void dump(int depth = 0, bool highlight_tokens = false);
	virtual const char* node_name() { return "ASTNode"; }
	void set_parent(ASTNode* parent);
	ASTNode* parent();
	void add_child(ASTNode* node);
	std::vector<ASTNode*> children();
	ASTNode* last_child() { return m_children[m_children.size() - 1]; }
	void remove_last_child() { m_children.pop_back(); }
	void clear_children() { m_children.clear(); }
	template<typename T>
	bool is() { return dynamic_cast<T*>(this) != nullptr; }
	template<typename T>
	T* as() { assert(is<T>());  return dynamic_cast<T*>(this); }
	void set_token(LexerToken* value) { m_token = value; }
	LexerToken* token() { return m_token; }
private:
	virtual void print_custom_data() {};
	ASTNode* m_parent;
	std::vector<ASTNode*> m_children{};
	LexerToken* m_token;
};