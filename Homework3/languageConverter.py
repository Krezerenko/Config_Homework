import math

from lark import Lark, Transformer

G = r'''
NAME: /[_A-Z][_a-zA-Z0-9]*/
NUM: /[0-9]+(\.[0-9]+)?/
LCOMMENT: /C .*/
MLCOMMENT: /<#(.|\n)*#>/
COMMENT: LCOMMENT | MLCOMMENT
WS: /\s+/
%ignore WS | COMMENT

pair: NAME ":" value
dict: "$[" ((pair ",")* pair)? "]"
value: dict | NUM | constexpr
operand: NAME | NUM | operation
plus: "+" operand operand
min: "min()" operand operand
sqrt: "sqrt()" operand
operation: plus | min | sqrt
constexpr: "@(" operand ")"
assignable: NUM | constexpr | dict
translation: assignable "->" NAME
line: (translation | dict) ";"

start: line*
'''

class LearnTree(Transformer):
    def __init__(self):
        super().__init__()
        self.variables = {}
    NAME = str
    NUM = float
    def translation(self, args):
        self.variables[args[1]] = args[0]
        return f"\"{args[1]}\" : {args[0]}"
    def operand(self, item):
        if isinstance(item[0], float):
            return item[0]
        if isinstance(item[0], str):
            return self.variables[item[0]]
        return item
    def plus(self, operands):
        return operands[0] + operands[1]
    def min(self, operands):
        return min(operands[0], operands[1])
    def sqrt(self, operand):
        return math.sqrt(operand[0])
    def operation(self, result):
        return result[0]
    def assignable(self, item):
        return item[0]
    def constexpr(self, result):
        return result[0]
    def value(self, item):
        return item[0]
    def pair(self, args):
        self.variables[args[0]] = args[1]
        return args[0], args[1]
    def dict(self, pairs):
        out = ""
        if len(pairs) > 0:
            for i in range(len(pairs) - 1):
                pair = pairs[i]
                out += f"\"{pair[0]}\" : {pair[1]}, "
                self.variables.pop(pair[0])
            pair = pairs[len(pairs) - 1]
            out += f"\"{pair[0]}\" : {pair[1]}"
            self.variables.pop(pair[0])

        out = "{" + out + "}"
        return out
    def line(self, line):
        return line[0]
    def start(self, lines):
        return '{' + ", ".join(lines) + '}'


learn_parser = Lark(G)
ex = r'''
7 -> Test1;
@(+ Test1 1) -> Test2;
3.14519 -> Good;
$[Car : @(+ min() 7 sqrt() 2.5 Good), Wheels : @(Car)] -> Bad;
$[];
'''

print(LearnTree().transform(learn_parser.parse(ex)))