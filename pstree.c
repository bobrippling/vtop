#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <sys/types.h>

#include "pstree.h"

#include "ps.h"
#include "proc.h"
#include "mem.h"

struct tree_entry
{
	struct process *p;
	struct tree_entry **children;
	size_t nchildren;
};

struct pstree
{
	struct tree_entry *entries;
	struct tree_entry **roots;
	size_t nroots;
};

pstree *pstree_new(struct ps *ps)
{
	const size_t n = ps_count(ps);
	pstree *tree = xmalloc(sizeof *tree);

	memset(tree, 0, sizeof(*tree));
	tree->entries = xcalloc(n, sizeof(*tree->entries));

	struct process *p;
	size_t i;
	/* fill in entries */
	for(i = 0; (p = ps_get_index(ps, i)); i++){
		tree->entries[i].p = p;

		size_t parentidx;
		if(ps_get_pid(ps, p->ppid, &parentidx)){
			struct tree_entry *parent = &tree->entries[parentidx];

			parent->nchildren++;
			parent->children = xrealloc(parent->children, parent->nchildren * sizeof(*parent->children));
			parent->children[parent->nchildren - 1] = &tree->entries[i];
		}else{
			tree->nroots++;
			tree->roots = xrealloc(tree->roots, tree->nroots * sizeof(*tree->roots));
			tree->roots[tree->nroots - 1] = &tree->entries[i];
		}
	}
	assert(i == n);

	return tree;
}

/* returns true when found */
static bool pstree_iter_r(
		struct tree_entry **ents,
		const size_t nents,
		size_t *const idx,
		struct process **const p,
		size_t *const indent)
{
	for(size_t i = 0; i < nents; i++){
		if(*idx == 0){
			*p = ents[i]->p;
			return true;
		}
		--*idx;

		if(pstree_iter_r(ents[i]->children, ents[i]->nchildren, idx, p, indent)){
			++*indent;
			return true;
		}
	}

	return false;
}

void pstree_get(pstree *tree, size_t i, struct process **const p, size_t *const indent)
{
	*indent = 0;
	*p = NULL;
	pstree_iter_r(tree->roots, tree->nroots, &i, p, indent);
}

void pstree_free(pstree *tree)
{
	free(tree->entries);
	free(tree->roots);
	free(tree);
}
