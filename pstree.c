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
	size_t nentries, nroots;
};

static int tree_entry_cmp(const void *va, const void *vb)
{
	const struct tree_entry *a = va, *b = vb;

	return a->p->pid - b->p->pid;
}

static void pstree_sort(struct tree_entry **ents, size_t nents)
{
	if(!ents)
		return;

	for(size_t i = 0; i < nents; i++)
		pstree_sort(ents[i]->children, ents[i]->nchildren);

	qsort(ents, nents, sizeof(*ents), tree_entry_cmp);
}

pstree *pstree_new(struct ps *ps)
{
	pstree *tree = xmalloc(sizeof *tree);

	memset(tree, 0, sizeof(*tree));
	tree->nentries = ps_count(ps);
	tree->entries = xcalloc(tree->nentries, sizeof(*tree->entries));

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
	assert(i == tree->nentries);

	pstree_sort(tree->roots, tree->nroots);

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
	for(size_t i = 0; i < tree->nentries; i++){
		struct tree_entry *ent = &tree->entries[i];
		free(ent->children);
	}

	free(tree->entries);
	free(tree->roots);
	free(tree);
}
