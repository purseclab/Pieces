# !/user/bin/env python
# -*- coding:utf-8 -*-
# Author: Aolin Ding
"""
ASTGerator.py

This module generate the symbolic expression of selected binary code.
Then it will convert the symbolic expression into an abstract syntax tree(AST)
and graph it into a dot image.
"""

###############################################################################
# Convert the symbolic expression into an AST
###############################################################################

import time
from anytree import Node, RenderTree, LevelOrderIter
from anytree.exporter import DotExporter
from anytree.exporter import UniqueDotExporter


class ASTGenerator:
    # Invalid types will be found and removed for tree simplification
    invalid_types = tuple()
    leaf_types = (int, float, str, invalid_types)

    # Define the valid and invalid operators based on Claripy fp operators
    # Add "Extract" if care about fp32 to 16 type changes
    valid_operator = ["fpAdd", "fpSub", "fpMul", "fpDiv", "fpNeg", "fpAbs"]
    # valid_matrix_operator = tuple("fpAdd", "fpSub", "fpMul", "fpDiv", "fpNeg", "fpAbs")
    invalid_operator = [
        "BVS", "fpToFP", "FPV", "fpToFPUnsigned", "fpToIEEEBV", "fpFP",
        "fpToSBV", "fpToUBV"
        # "fpToSBV", "fpToUBV", "Concat", "Extract", "BVV"
    ]

    def __init__(self, root):
        self.root = root

    def get_value(self, node):
        """Return node value bases on its name"""
        return node.name

    def get_operator(self, node):
        """Return node operator value"""
        return node.name.op

    def is_valid(self, node):
        """Test purpose"""
        return self.get_value(node) is not None

    # def is_valid_value_type(self, node):
    #     """Check if the node's value has valid type"""
    #     return isinstance(self.get_value(node), self.invalid_types)

    def is_defined_operator(self, node):
        # return node.name in self.valid_operator
        return node.name in self.valid_operator and len(node.children) == 2 and self.is_defined_leaf(node.children[0]) and self.is_defined_leaf(node.children[1])

    def is_defined_leaf(self, node):
        """Check if the node is already the defined leaf type"""
        return isinstance(self.get_value(node), self.leaf_types)

    def is_symbolic_expression(self, node):
        """Test purpose"""
        print(self.get_operator(node))
        return isinstance(self.get_operator(node),
                          type(self.get_value(self.root)))

    def has_operator(self, node):
        """Test purpose"""
        return issubclass(type(self.get_operator(node)),
                          type(self.get_value(node)))

    def build_sym_AST(self, start_node):
        """Traverse and generate the symbolic abstract syntax tree (AST)"""
        for i in range(3):
            child_node = Node(start_node.name.args[i], parent=start_node)

            try:
                if self.is_defined_leaf(child_node) is False:
                    self.build_sym_AST(child_node)
            except IndexError:
                pass
            except AttributeError:
                """
                Attribute Error indicates the current node is a
                leaf node, and add its type into invalid types.
                """
                exception_type = type(self.get_value(child_node))
                """
                Check if tuple contains this type by string since types are not
                iterable and only add new exception type into invalid types.
                """
                if str(exception_type) in str(self.invalid_types):
                    pass
                else:
                    temp_exception_type = exception_type
                    self.invalid_types = (self.invalid_types,
                                          temp_exception_type)
                    # print(self.invalid_types)
            continue

    def print_tree_by_name(self, start_node):
        """Walk through the tree and print out argument variables NAMES"""
        print("")
        for pre, fill, node in RenderTree(start_node):
            print("%s%s" % (pre, node.name))
        print("")

    def print_tree_by_operator(self, start_node):
        """Walk throught the tree and print out argument variables OPERATORS"""
        print("")
        for pre, fill, node in RenderTree(start_node):
            if len(node.children) != 0:
                print("%s%s     %s" %
                      (pre, self.get_operator(node), node.depth))
            else:
                print("%s%s     %s" % (pre, self.get_value(node), node.depth))
        print("")

    def simplify_tree_by_node(self, start_node):
        """Delete those nodes whose VALUES are None or are out of index range,
        and delete those unnecessary nodes whose VALUES are Enum or FP types.
        """
        for pre, fill, node in RenderTree(start_node):
            try:
                # print("%s%s" % (pre, get_value(node)))
                if isinstance(self.get_value(node), self.invalid_types):
                    node.parent = None
            except IndexError:
                node.parent = None
            except AttributeError:
                node.parent = None
            continue

    def update_tree_by_operator(self, start_node):
        """Replace parent node NAME with its OPERATOR to simplify dot graph"""
        for pre, fill, node in RenderTree(start_node):
            if len(node.children) != 0:
                node.name = self.get_operator(node)
            else:
                node.name = self.get_value(node)

    def merge_duplicated_operator(self, start_node):
        """If parent node and children node have the same operator, merge them"""
        for pre, fill, node in RenderTree(start_node):
            if node.name in self.valid_operator and \
                    node.is_root is False and \
                    node.name == node.parent.name:
                for child_node in node.children:
                    child_node.parent = node.parent
                node.parent = None

    def get_first_symbolic_node(self, start_node):
        """Get the first valid symbolic node whose operator is in valid list"""
        """If can not find until reaching leaf node, then get the leaf node"""
        for pre, fill, node in RenderTree(start_node):
            if (self.is_defined_leaf(node) is False
                    and self.get_operator(node) in self.valid_operator):
                node.parent = None
                return node
            elif self.is_defined_leaf(node) is True and isinstance(
                    self.get_value(node), int) is False:
                node.parent = None
                return node
        return None

    def remove_invalid_operator(self, start_node):
        """Remove invalid operator based on predefined list"""
        for pre, fill, node in RenderTree(start_node):
            if (self.is_defined_leaf(node) is False
                    and self.get_operator(node) in self.invalid_operator):
                for child_node in node.children:
                    child_node.parent = node.parent
                    self.remove_invalid_operator(child_node)
                node.parent = None

    def store_ast_image(self, start_node, imageName):
        """store the abstract syntax tree into an image"""
        if imageName is not None:
            self.imageName = imageName
            UniqueDotExporter(start_node).to_picture(imageName)
        else:
            print("The dot image is not named and will not be stored.")

    def attach_node(self, root, node, position):
        """Testing purpose, not used"""

    def merge_ast(self, root1, root2=[], position=[], replace=True):
        """merge two ASTs, AST2 will be attached at anotated postions on AST1"""
        # if len(position) > 1:
        #     for p in range(len(position) - 1):
        #         temp = self.attach_AST(root1, root2, position[p])
        #         self.store_ast_image(temp, "temp.png")
        #         root = self.attach_AST(temp, root2, position[p + 1])
        # else:
        #     root = self.attach_AST(root1, root2, position[0])
        # return root

        for i in range(len(position)):
            # print(i)
            p = 0
            for node in LevelOrderIter(root1):
                if p == position[i]:
                    print(node.name)
                    if replace:
                        root2[i].parent = node.parent
                        node.parent = None
                    else:
                        root2[i].parent = node
                    break
                p += 1
        # After the AST merging, merge the newly duplicated operator
        self.merge_duplicated_operator(root1)

    def generate_ast(self, root_node, imageName=None):
        """All-in-one function: generate and optimize the abstract syntax tree,
        and (optionaly) store as an image"""
        self.build_sym_AST(root_node)
        self.simplify_tree_by_node(root_node)
        self.print_tree_by_name(root_node)

        # The first symbolic node will be the new root
        new_root = self.get_first_symbolic_node(root_node)
        self.remove_invalid_operator(new_root)
        self.print_tree_by_operator(new_root)
        self.update_tree_by_operator(new_root)
        self.merge_duplicated_operator(new_root)
        if self.is_single_node_tree(new_root) is False:
        # Only save the tree image when the tree is not single node
            self.store_ast_image(new_root, imageName)
        return new_root

    def generate_valid_matrix_operator_ast(self, root_node, imageName=None):
        new_root = self.generate_ast(root_node, imageName)
        return new_root, self.is_defined_operator(new_root), new_root.name


    def is_single_node_tree(self, root_node):
        return True if root_node.is_root and root_node.is_leaf else False

