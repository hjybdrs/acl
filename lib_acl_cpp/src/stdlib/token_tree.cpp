#include "acl_stdafx.hpp"
#ifndef ACL_PREPARE_COMPILE
#include "acl_cpp/stdlib/token_tree.hpp"
#endif

namespace acl
{

token_node::token_node(void)
: me_(NULL)
, tree_(NULL)
{
}

token_node::~token_node(void)
{
}

void token_node::set_node(ACL_TOKEN* token, token_tree* tree)
{
	me_   = token;
	tree_ = tree;
	acl_token_name(token, key_.vstring());
}

void* token_node::get_ctx(void) const
{
	return  me_->ctx;
}

//////////////////////////////////////////////////////////////////////////////

token_tree::token_tree(void)
: iter_(NULL)
{
	tree_ = acl_token_new();
}

token_tree::~token_tree(void)
{
	acl_token_tree_destroy(tree_);
	delete iter_;
}

bool token_tree::insert(const char* key, void* ctx)
{
	if (acl_token_tree_word_match(tree_, key) != NULL) {
		return false;
	}
	if (acl_token_tree_add(tree_, key, ACL_TOKEN_F_STOP, ctx) == NULL) {
		return false;
	}

	return true;
}

void* token_tree::remove(const char* key)
{
	return acl_token_tree_word_remove(tree_, key);
}

const token_node* token_tree::search(const char* haystack)
{
	ACL_TOKEN* token = acl_token_tree_word_match(tree_, haystack);
	if (token == NULL) {
		return NULL;
	}

	node_.set_node(token, this);
	return &node_;
}

const token_node* token_tree::match(const char** text, const char* delimiters,
	const char* delimiters_tab)
{
	ACL_TOKEN* token = acl_token_tree_match(tree_, text,
			delimiters, delimiters_tab);
	if (token == NULL) {
		return NULL;
	}
	node_.set_node(token, this);
	return &node_;
}

char* token_tree::create_delimiters_tab(const char* delimiters)
{
	return acl_token_delim_tab_new(delimiters);
}

void token_tree::free_delimiters_tab(char* delimiters_tab)
{
	acl_token_delim_tab_free(delimiters_tab);
}

const token_node* token_tree::first_node(void)
{
	if (iter_ == NULL) {
		iter_ = (ACL_ITER *) acl_mymalloc(sizeof(ACL_ITER));
	}

	ACL_TOKEN* token = tree_->iter_head(iter_, tree_);
	if (token == NULL) {
		return NULL;
	}
	node_.set_node(token, this);
	return &node_;
}

const token_node* token_tree::next_node(void)
{
	if (iter_ == NULL) {
		logger_error("call first_node first!");
		return NULL;
	}

	ACL_TOKEN* token = tree_->iter_next(iter_, tree_);
	if (token == NULL) {
		return NULL;
	}
	node_.set_node(token, this);
	return &node_;
}

} // namespace acl