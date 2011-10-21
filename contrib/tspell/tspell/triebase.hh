/*
 * Copyright (C) 2011 Dmitry Marakasov
 *
 * This file is part of tspell.
 *
 * tspell is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tspell is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tspell.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TSPELL_TRIEBASE_HH
#define TSPELL_TRIEBASE_HH

namespace TSpell {

template<class Char>
struct Node {
	Node* parent;
	Node* next;
	Node* child;
	Char ch;
	bool data;

	Node(Node* p, Char c) : parent(p), next(NULL), child(NULL), ch(c), data(false) {
	}
};

template<class Char, class Appender>
class TrieBase {
protected:
	typedef Node<Char> Node;

protected:
	Node* root_;

private:
	void Insert(Node* parent, Node** start, const Char* string, size_t length) {
		Node** current = start;

		for (; *current != NULL && (*current)->ch != *string; current = &((*current)->next)) {
			/* empty */
		}

		if (*current == NULL)
			*current = new Node(parent, *string);

		if (length == 1) {
			(*current)->data = true;
		} else {
			Insert(*current, &((*current)->child), string + 1, length - 1);
		}
	}

	/* XXX: const */
	bool FindExact(Node* start, const Char* string, size_t length) const {
		Node* current = start;

		for (; current != NULL && current->ch != *string; current = current->next) {
			/* empty */
		}

		if (current == NULL)
			return false;

		if (length == 1) {
			return current->data;
		} else {
			return FindExact(current->child, string + 1, length - 1);
		}
	}

	void FindApprox(Node* last, Node* current, const Char* string, size_t length, int distance, Appender& appender) const {
		/* remove character, we can do it regardless of position in a trie given we have distance */
		if (length > 0 && distance > 0)
			FindApprox(last, current, string + 1, length - 1, distance - 1, appender);

		/* match */
		if (length == 0 && last && last->data)
			appender.Append(last);

		/* we won't be able to proceed */
		if (distance == 0 && length == 0)
			return;

		for (; current != NULL; current = current->next) {
			/* normal path */
			if (length > 0 && current->ch == *string)
				FindApprox(current, current->child, string + 1, length - 1, distance, appender);

			/* change character */
			if (distance > 0 && length > 0 && current->ch != *string)
				FindApprox(current, current->child, string + 1, length - 1, distance - 1, appender);

			/* add character */
			if (distance > 0)
				FindApprox(current, current->child, string, length, distance - 1, appender);
		}
	}

	void Destroy(Node* node) {
		if (node->child)
			Destroy(node->child);
		if (node->next)
			Destroy(node->next);
		delete node;
	}

protected:
	TrieBase() : root_(NULL) {
	}

	~TrieBase() {
		if (root_)
			Destroy(root_);
	}

	void Insert(const Char* string, size_t length) {
		if (length > 0)
			Insert(NULL, &root_, string, length);
	}

	bool FindExact(const Char* string, size_t length) const {
		if (length == 0 || !root_)
			return false;

		return FindExact(root_, string, length);
	}

	void FindApprox(const Char* string, size_t length, int distance, Appender& appender) const {
		if (root_)
			FindApprox(NULL, root_, string, length, distance, appender);
	}
};

}

#endif
