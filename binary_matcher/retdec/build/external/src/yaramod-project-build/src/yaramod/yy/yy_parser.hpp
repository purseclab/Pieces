// A Bison parser, made by GNU Bison 3.0.4.

// Skeleton interface for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

/**
 ** \file /home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.hpp
 ** Define the  yaramod::yy ::parser class.
 */

// C++ LALR(1) parser skeleton written by Akim Demaille.

#ifndef YY_YY_HOME_SLOAN_DISPATCH_LINUX_RETDEC_BUILD_EXTERNAL_SRC_YARAMOD_PROJECT_BUILD_SRC_YARAMOD_YY_YY_PARSER_HPP_INCLUDED
# define YY_YY_HOME_SLOAN_DISPATCH_LINUX_RETDEC_BUILD_EXTERNAL_SRC_YARAMOD_PROJECT_BUILD_SRC_YARAMOD_YY_YY_PARSER_HPP_INCLUDED
// //                    "%code requires" blocks.
#line 7 "parser/yy/parser.y" // lalr1.cc:377

#include <iterator>

#include "yaramod/types/expressions.h"
#include "yaramod/types/hex_string.h"
#include "yaramod/types/literal.h"
#include "yaramod/types/meta.h"
#include "yaramod/types/plain_string.h"
#include "yaramod/types/regexp.h"
#include "yaramod/types/rule.h"
#include "yaramod/utils/trie.h"

namespace yaramod {

class ParserDriver;

// We need to provide alias to this type because since bison 3.2
// types are enclosed in YY_RVREF() macro. The comma in template
// parameter list is however parsed by preprocessor first and
// therefore the preprocessor thinks we are passing it 2 arguments.
// We then get error like this:
//
// error: too many arguments provided to function-like macro invocation
//
// See https://github.com/avast/yaramod/issues/11 for further information.
using RegexpRangePair = std::pair<nonstd::optional<std::uint64_t>, nonstd::optional<std::uint64_t>>;

}

// Uncomment for debugging
// See also other occurrences of 'debugging' in this file and constructor of ParserDriver to enable it
#define YYDEBUG 1

#line 78 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.hpp" // lalr1.cc:377

# include <cassert>
# include <cstdlib> // std::abort
# include <iostream>
# include <stdexcept>
# include <string>
# include <vector>
# include "stack.hh"
# include "location.hh"
#include <typeinfo>
#ifndef YYASSERT
# include <cassert>
# define YYASSERT assert
#endif


#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

#line 61 "parser/yy/parser.y" // lalr1.cc:377
namespace  yaramod { namespace yy  {
#line 155 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.hpp" // lalr1.cc:377



  /// A char[S] buffer to store and retrieve objects.
  ///
  /// Sort of a variant, but does not keep track of the nature
  /// of the stored data, since that knowledge is available
  /// via the current state.
  template <size_t S>
  struct variant
  {
    /// Type of *this.
    typedef variant<S> self_type;

    /// Empty construction.
    variant ()
      : yytypeid_ (YY_NULLPTR)
    {}

    /// Construct and fill.
    template <typename T>
    variant (const T& t)
      : yytypeid_ (&typeid (T))
    {
      YYASSERT (sizeof (T) <= S);
      new (yyas_<T> ()) T (t);
    }

    /// Destruction, allowed only if empty.
    ~variant ()
    {
      YYASSERT (!yytypeid_);
    }

    /// Instantiate an empty \a T in here.
    template <typename T>
    T&
    build ()
    {
      YYASSERT (!yytypeid_);
      YYASSERT (sizeof (T) <= S);
      yytypeid_ = & typeid (T);
      return *new (yyas_<T> ()) T;
    }

    /// Instantiate a \a T in here from \a t.
    template <typename T>
    T&
    build (const T& t)
    {
      YYASSERT (!yytypeid_);
      YYASSERT (sizeof (T) <= S);
      yytypeid_ = & typeid (T);
      return *new (yyas_<T> ()) T (t);
    }

    /// Accessor to a built \a T.
    template <typename T>
    T&
    as ()
    {
      YYASSERT (*yytypeid_ == typeid (T));
      YYASSERT (sizeof (T) <= S);
      return *yyas_<T> ();
    }

    /// Const accessor to a built \a T (for %printer).
    template <typename T>
    const T&
    as () const
    {
      YYASSERT (*yytypeid_ == typeid (T));
      YYASSERT (sizeof (T) <= S);
      return *yyas_<T> ();
    }

    /// Swap the content with \a other, of same type.
    ///
    /// Both variants must be built beforehand, because swapping the actual
    /// data requires reading it (with as()), and this is not possible on
    /// unconstructed variants: it would require some dynamic testing, which
    /// should not be the variant's responsability.
    /// Swapping between built and (possibly) non-built is done with
    /// variant::move ().
    template <typename T>
    void
    swap (self_type& other)
    {
      YYASSERT (yytypeid_);
      YYASSERT (*yytypeid_ == *other.yytypeid_);
      std::swap (as<T> (), other.as<T> ());
    }

    /// Move the content of \a other to this.
    ///
    /// Destroys \a other.
    template <typename T>
    void
    move (self_type& other)
    {
      build<T> ();
      swap<T> (other);
      other.destroy<T> ();
    }

    /// Copy the content of \a other to this.
    template <typename T>
    void
    copy (const self_type& other)
    {
      build<T> (other.as<T> ());
    }

    /// Destroy the stored \a T.
    template <typename T>
    void
    destroy ()
    {
      as<T> ().~T ();
      yytypeid_ = YY_NULLPTR;
    }

  private:
    /// Prohibit blind copies.
    self_type& operator=(const self_type&);
    variant (const self_type&);

    /// Accessor to raw memory as \a T.
    template <typename T>
    T*
    yyas_ ()
    {
      void *yyp = yybuffer_.yyraw;
      return static_cast<T*> (yyp);
     }

    /// Const accessor to raw memory as \a T.
    template <typename T>
    const T*
    yyas_ () const
    {
      const void *yyp = yybuffer_.yyraw;
      return static_cast<const T*> (yyp);
     }

    union
    {
      /// Strongest alignment constraints.
      long double yyalign_me;
      /// A buffer large enough to store any of the semantic values.
      char yyraw[S];
    } yybuffer_;

    /// Whether the content is built: if defined, the name of the stored type.
    const std::type_info *yytypeid_;
  };


  /// A Bison parser.
  class  Parser 
  {
  public:
#ifndef YYSTYPE
    /// An auxiliary type to compute the largest semantic type.
    union union_type
    {
      // condition
      // expression
      // primary_expression
      // range
      // for_expression
      // integer_set
      // string_set
      // identifier
      char dummy1[sizeof(Expression::Ptr)];

      // boolean
      // regexp_greedy
      char dummy2[sizeof(bool)];

      // hex_or
      // hex_jump
      char dummy3[sizeof(std::shared_ptr<yaramod::HexStringUnit>)];

      // regexp_or
      // regexp_repeat
      // regexp_single
      char dummy4[sizeof(std::shared_ptr<yaramod::RegexpUnit>)];

      // strings
      // strings_body
      char dummy5[sizeof(std::shared_ptr<yaramod::Rule::StringsTrie>)];

      // string
      // regexp
      // regexp_body
      char dummy6[sizeof(std::shared_ptr<yaramod::String>)];

      // "/"
      // "string literal"
      // "integer"
      // "float"
      // "string identifier"
      // "string wildcard"
      // "string length"
      // "string offset"
      // "string count"
      // "identifier"
      // "fixed-width integer function"
      // "regexp character"
      // "regexp class"
      char dummy7[sizeof(std::string)];

      // string_mods
      char dummy8[sizeof(std::uint32_t)];

      // "hex string integer"
      char dummy9[sizeof(std::uint64_t)];

      // "hex string nibble"
      char dummy10[sizeof(std::uint8_t)];

      // integer_enumeration
      // string_enumeration
      // arguments
      char dummy11[sizeof(std::vector<Expression::Ptr>)];

      // hex_or_body
      char dummy12[sizeof(std::vector<std::shared_ptr<yaramod::HexString>>)];

      // hex_string
      // hex_string_edge
      // hex_byte
      // hex_string_body
      char dummy13[sizeof(std::vector<std::shared_ptr<yaramod::HexStringUnit>>)];

      // regexp_concat
      char dummy14[sizeof(std::vector<std::shared_ptr<yaramod::RegexpUnit>>)];

      // tags
      // tag_list
      char dummy15[sizeof(std::vector<std::string>)];

      // metas
      // metas_body
      char dummy16[sizeof(std::vector<yaramod::Meta>)];

      // literal
      char dummy17[sizeof(yaramod::Literal)];

      // "regexp range"
      char dummy18[sizeof(yaramod::RegexpRangePair)];

      // rule
      char dummy19[sizeof(yaramod::Rule)];

      // rule_mod
      char dummy20[sizeof(yaramod::Rule::Modifier)];
};

    /// Symbol semantic values.
    typedef variant<sizeof(union_type)> semantic_type;
#else
    typedef YYSTYPE semantic_type;
#endif
    /// Symbol locations.
    typedef location location_type;

    /// Syntax errors thrown from user actions.
    struct syntax_error : std::runtime_error
    {
      syntax_error (const location_type& l, const std::string& m);
      location_type location;
    };

    /// Tokens.
    struct token
    {
      enum yytokentype
      {
        END = 258,
        RANGE = 259,
        DOT = 260,
        LT = 261,
        GT = 262,
        LE = 263,
        GE = 264,
        EQ = 265,
        NEQ = 266,
        SHIFT_LEFT = 267,
        SHIFT_RIGHT = 268,
        MINUS = 269,
        PLUS = 270,
        MULTIPLY = 271,
        DIVIDE = 272,
        MODULO = 273,
        BITWISE_XOR = 274,
        BITWISE_AND = 275,
        BITWISE_OR = 276,
        BITWISE_NOT = 277,
        LP = 278,
        RP = 279,
        LCB = 280,
        RCB = 281,
        ASSIGN = 282,
        COLON = 283,
        COMMA = 284,
        PRIVATE = 285,
        GLOBAL = 286,
        RULE = 287,
        META = 288,
        STRINGS = 289,
        CONDITION = 290,
        ASCII = 291,
        NOCASE = 292,
        WIDE = 293,
        FULLWORD = 294,
        XOR = 295,
        BOOL_TRUE = 296,
        BOOL_FALSE = 297,
        IMPORT_MODULE = 298,
        NOT = 299,
        AND = 300,
        OR = 301,
        ALL = 302,
        ANY = 303,
        OF = 304,
        THEM = 305,
        FOR = 306,
        ENTRYPOINT = 307,
        OP_AT = 308,
        OP_IN = 309,
        FILESIZE = 310,
        CONTAINS = 311,
        MATCHES = 312,
        SLASH = 313,
        STRING_LITERAL = 314,
        INTEGER = 315,
        DOUBLE = 316,
        STRING_ID = 317,
        STRING_ID_WILDCARD = 318,
        STRING_LENGTH = 319,
        STRING_OFFSET = 320,
        STRING_COUNT = 321,
        ID = 322,
        INTEGER_FUNCTION = 323,
        HEX_OR = 324,
        LSQB = 325,
        RSQB = 326,
        HEX_WILDCARD = 327,
        DASH = 328,
        HEX_NIBBLE = 329,
        HEX_INTEGER = 330,
        REGEXP_OR = 331,
        REGEXP_ITER = 332,
        REGEXP_PITER = 333,
        REGEXP_OPTIONAL = 334,
        REGEXP_START_OF_LINE = 335,
        REGEXP_END_OF_LINE = 336,
        REGEXP_ANY_CHAR = 337,
        REGEXP_WORD_CHAR = 338,
        REGEXP_NON_WORD_CHAR = 339,
        REGEXP_SPACE = 340,
        REGEXP_NON_SPACE = 341,
        REGEXP_DIGIT = 342,
        REGEXP_NON_DIGIT = 343,
        REGEXP_WORD_BOUNDARY = 344,
        REGEXP_NON_WORD_BOUNDARY = 345,
        REGEXP_CHAR = 346,
        REGEXP_RANGE = 347,
        REGEXP_CLASS = 348,
        UNARY_MINUS = 349
      };
    };

    /// (External) token type, as returned by yylex.
    typedef token::yytokentype token_type;

    /// Symbol type: an internal symbol number.
    typedef int symbol_number_type;

    /// The symbol type number to denote an empty symbol.
    enum { empty_symbol = -2 };

    /// Internal symbol number for tokens (subsumed by symbol_number_type).
    typedef unsigned char token_number_type;

    /// A complete symbol.
    ///
    /// Expects its Base type to provide access to the symbol type
    /// via type_get().
    ///
    /// Provide access to semantic value and location.
    template <typename Base>
    struct basic_symbol : Base
    {
      /// Alias to Base.
      typedef Base super_type;

      /// Default constructor.
      basic_symbol ();

      /// Copy constructor.
      basic_symbol (const basic_symbol& other);

      /// Constructor for valueless symbols, and symbols from each type.

  basic_symbol (typename Base::kind_type t, const location_type& l);

  basic_symbol (typename Base::kind_type t, const Expression::Ptr v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const bool v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const std::shared_ptr<yaramod::HexStringUnit> v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const std::shared_ptr<yaramod::RegexpUnit> v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const std::shared_ptr<yaramod::Rule::StringsTrie> v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const std::shared_ptr<yaramod::String> v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const std::string v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const std::uint32_t v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const std::uint64_t v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const std::uint8_t v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const std::vector<Expression::Ptr> v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const std::vector<std::shared_ptr<yaramod::HexString>> v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const std::vector<std::shared_ptr<yaramod::HexStringUnit>> v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const std::vector<std::shared_ptr<yaramod::RegexpUnit>> v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const std::vector<std::string> v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const std::vector<yaramod::Meta> v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const yaramod::Literal v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const yaramod::RegexpRangePair v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const yaramod::Rule v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const yaramod::Rule::Modifier v, const location_type& l);


      /// Constructor for symbols with semantic value.
      basic_symbol (typename Base::kind_type t,
                    const semantic_type& v,
                    const location_type& l);

      /// Destroy the symbol.
      ~basic_symbol ();

      /// Destroy contents, and record that is empty.
      void clear ();

      /// Whether empty.
      bool empty () const;

      /// Destructive move, \a s is emptied into this.
      void move (basic_symbol& s);

      /// The semantic value.
      semantic_type value;

      /// The location.
      location_type location;

    private:
      /// Assignment operator.
      basic_symbol& operator= (const basic_symbol& other);
    };

    /// Type access provider for token (enum) based symbols.
    struct by_type
    {
      /// Default constructor.
      by_type ();

      /// Copy constructor.
      by_type (const by_type& other);

      /// The symbol type as needed by the constructor.
      typedef token_type kind_type;

      /// Constructor from (external) token numbers.
      by_type (kind_type t);

      /// Record that this symbol is empty.
      void clear ();

      /// Steal the symbol type from \a that.
      void move (by_type& that);

      /// The (internal) type number (corresponding to \a type).
      /// \a empty when empty.
      symbol_number_type type_get () const;

      /// The token.
      token_type token () const;

      /// The symbol type.
      /// \a empty_symbol when empty.
      /// An int, not token_number_type, to be able to store empty_symbol.
      int type;
    };

    /// "External" symbols: returned by the scanner.
    typedef basic_symbol<by_type> symbol_type;

    // Symbol constructors declarations.
    static inline
    symbol_type
    make_END (const location_type& l);

    static inline
    symbol_type
    make_RANGE (const location_type& l);

    static inline
    symbol_type
    make_DOT (const location_type& l);

    static inline
    symbol_type
    make_LT (const location_type& l);

    static inline
    symbol_type
    make_GT (const location_type& l);

    static inline
    symbol_type
    make_LE (const location_type& l);

    static inline
    symbol_type
    make_GE (const location_type& l);

    static inline
    symbol_type
    make_EQ (const location_type& l);

    static inline
    symbol_type
    make_NEQ (const location_type& l);

    static inline
    symbol_type
    make_SHIFT_LEFT (const location_type& l);

    static inline
    symbol_type
    make_SHIFT_RIGHT (const location_type& l);

    static inline
    symbol_type
    make_MINUS (const location_type& l);

    static inline
    symbol_type
    make_PLUS (const location_type& l);

    static inline
    symbol_type
    make_MULTIPLY (const location_type& l);

    static inline
    symbol_type
    make_DIVIDE (const location_type& l);

    static inline
    symbol_type
    make_MODULO (const location_type& l);

    static inline
    symbol_type
    make_BITWISE_XOR (const location_type& l);

    static inline
    symbol_type
    make_BITWISE_AND (const location_type& l);

    static inline
    symbol_type
    make_BITWISE_OR (const location_type& l);

    static inline
    symbol_type
    make_BITWISE_NOT (const location_type& l);

    static inline
    symbol_type
    make_LP (const location_type& l);

    static inline
    symbol_type
    make_RP (const location_type& l);

    static inline
    symbol_type
    make_LCB (const location_type& l);

    static inline
    symbol_type
    make_RCB (const location_type& l);

    static inline
    symbol_type
    make_ASSIGN (const location_type& l);

    static inline
    symbol_type
    make_COLON (const location_type& l);

    static inline
    symbol_type
    make_COMMA (const location_type& l);

    static inline
    symbol_type
    make_PRIVATE (const location_type& l);

    static inline
    symbol_type
    make_GLOBAL (const location_type& l);

    static inline
    symbol_type
    make_RULE (const location_type& l);

    static inline
    symbol_type
    make_META (const location_type& l);

    static inline
    symbol_type
    make_STRINGS (const location_type& l);

    static inline
    symbol_type
    make_CONDITION (const location_type& l);

    static inline
    symbol_type
    make_ASCII (const location_type& l);

    static inline
    symbol_type
    make_NOCASE (const location_type& l);

    static inline
    symbol_type
    make_WIDE (const location_type& l);

    static inline
    symbol_type
    make_FULLWORD (const location_type& l);

    static inline
    symbol_type
    make_XOR (const location_type& l);

    static inline
    symbol_type
    make_BOOL_TRUE (const location_type& l);

    static inline
    symbol_type
    make_BOOL_FALSE (const location_type& l);

    static inline
    symbol_type
    make_IMPORT_MODULE (const location_type& l);

    static inline
    symbol_type
    make_NOT (const location_type& l);

    static inline
    symbol_type
    make_AND (const location_type& l);

    static inline
    symbol_type
    make_OR (const location_type& l);

    static inline
    symbol_type
    make_ALL (const location_type& l);

    static inline
    symbol_type
    make_ANY (const location_type& l);

    static inline
    symbol_type
    make_OF (const location_type& l);

    static inline
    symbol_type
    make_THEM (const location_type& l);

    static inline
    symbol_type
    make_FOR (const location_type& l);

    static inline
    symbol_type
    make_ENTRYPOINT (const location_type& l);

    static inline
    symbol_type
    make_OP_AT (const location_type& l);

    static inline
    symbol_type
    make_OP_IN (const location_type& l);

    static inline
    symbol_type
    make_FILESIZE (const location_type& l);

    static inline
    symbol_type
    make_CONTAINS (const location_type& l);

    static inline
    symbol_type
    make_MATCHES (const location_type& l);

    static inline
    symbol_type
    make_SLASH (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_STRING_LITERAL (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_INTEGER (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_DOUBLE (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_STRING_ID (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_STRING_ID_WILDCARD (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_STRING_LENGTH (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_STRING_OFFSET (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_STRING_COUNT (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_ID (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_INTEGER_FUNCTION (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_HEX_OR (const location_type& l);

    static inline
    symbol_type
    make_LSQB (const location_type& l);

    static inline
    symbol_type
    make_RSQB (const location_type& l);

    static inline
    symbol_type
    make_HEX_WILDCARD (const location_type& l);

    static inline
    symbol_type
    make_DASH (const location_type& l);

    static inline
    symbol_type
    make_HEX_NIBBLE (const std::uint8_t& v, const location_type& l);

    static inline
    symbol_type
    make_HEX_INTEGER (const std::uint64_t& v, const location_type& l);

    static inline
    symbol_type
    make_REGEXP_OR (const location_type& l);

    static inline
    symbol_type
    make_REGEXP_ITER (const location_type& l);

    static inline
    symbol_type
    make_REGEXP_PITER (const location_type& l);

    static inline
    symbol_type
    make_REGEXP_OPTIONAL (const location_type& l);

    static inline
    symbol_type
    make_REGEXP_START_OF_LINE (const location_type& l);

    static inline
    symbol_type
    make_REGEXP_END_OF_LINE (const location_type& l);

    static inline
    symbol_type
    make_REGEXP_ANY_CHAR (const location_type& l);

    static inline
    symbol_type
    make_REGEXP_WORD_CHAR (const location_type& l);

    static inline
    symbol_type
    make_REGEXP_NON_WORD_CHAR (const location_type& l);

    static inline
    symbol_type
    make_REGEXP_SPACE (const location_type& l);

    static inline
    symbol_type
    make_REGEXP_NON_SPACE (const location_type& l);

    static inline
    symbol_type
    make_REGEXP_DIGIT (const location_type& l);

    static inline
    symbol_type
    make_REGEXP_NON_DIGIT (const location_type& l);

    static inline
    symbol_type
    make_REGEXP_WORD_BOUNDARY (const location_type& l);

    static inline
    symbol_type
    make_REGEXP_NON_WORD_BOUNDARY (const location_type& l);

    static inline
    symbol_type
    make_REGEXP_CHAR (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_REGEXP_RANGE (const yaramod::RegexpRangePair& v, const location_type& l);

    static inline
    symbol_type
    make_REGEXP_CLASS (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_UNARY_MINUS (const location_type& l);


    /// Build a parser object.
     Parser  (yaramod::ParserDriver& driver_yyarg);
    virtual ~ Parser  ();

    /// Parse.
    /// \returns  0 iff parsing succeeded.
    virtual int parse ();

#if YYDEBUG
    /// The current debugging stream.
    std::ostream& debug_stream () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging stream.
    void set_debug_stream (std::ostream &);

    /// Type for debugging levels.
    typedef int debug_level_type;
    /// The current debugging level.
    debug_level_type debug_level () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging level.
    void set_debug_level (debug_level_type l);
#endif

    /// Report a syntax error.
    /// \param loc    where the syntax error is found.
    /// \param msg    a description of the syntax error.
    virtual void error (const location_type& loc, const std::string& msg);

    /// Report a syntax error.
    void error (const syntax_error& err);

  private:
    /// This class is not copyable.
     Parser  (const  Parser &);
     Parser & operator= (const  Parser &);

    /// State numbers.
    typedef int state_type;

    /// Generate an error message.
    /// \param yystate   the state where the error occurred.
    /// \param yyla      the lookahead token.
    virtual std::string yysyntax_error_ (state_type yystate,
                                         const symbol_type& yyla) const;

    /// Compute post-reduction state.
    /// \param yystate   the current state
    /// \param yysym     the nonterminal to push on the stack
    state_type yy_lr_goto_state_ (state_type yystate, int yysym);

    /// Whether the given \c yypact_ value indicates a defaulted state.
    /// \param yyvalue   the value to check
    static bool yy_pact_value_is_default_ (int yyvalue);

    /// Whether the given \c yytable_ value indicates a syntax error.
    /// \param yyvalue   the value to check
    static bool yy_table_value_is_error_ (int yyvalue);

    static const short int yypact_ninf_;
    static const signed char yytable_ninf_;

    /// Convert a scanner token number \a t to a symbol number.
    static token_number_type yytranslate_ (token_type t);

    // Tables.
  // YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
  // STATE-NUM.
  static const short int yypact_[];

  // YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
  // Performed when YYTABLE does not specify something else to do.  Zero
  // means the default is an error.
  static const unsigned char yydefact_[];

  // YYPGOTO[NTERM-NUM].
  static const short int yypgoto_[];

  // YYDEFGOTO[NTERM-NUM].
  static const short int yydefgoto_[];

  // YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
  // positive, shift that token.  If negative, reduce the rule whose
  // number is the opposite.  If YYTABLE_NINF, syntax error.
  static const short int yytable_[];

  static const unsigned char yycheck_[];

  // YYSTOS[STATE-NUM] -- The (internal number of the) accessing
  // symbol of state STATE-NUM.
  static const unsigned char yystos_[];

  // YYR1[YYN] -- Symbol number of symbol that rule YYN derives.
  static const unsigned char yyr1_[];

  // YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.
  static const unsigned char yyr2_[];


    /// Convert the symbol name \a n to a form suitable for a diagnostic.
    static std::string yytnamerr_ (const char *n);


    /// For a symbol, its name in clear.
    static const char* const yytname_[];
#if YYDEBUG
  // YYRLINE[YYN] -- Source line where rule number YYN was defined.
  static const unsigned short int yyrline_[];
    /// Report on the debug stream that the rule \a r is going to be reduced.
    virtual void yy_reduce_print_ (int r);
    /// Print the state stack on the debug stream.
    virtual void yystack_print_ ();

    // Debugging.
    int yydebug_;
    std::ostream* yycdebug_;

    /// \brief Display a symbol type, value and location.
    /// \param yyo    The output stream.
    /// \param yysym  The symbol.
    template <typename Base>
    void yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const;
#endif

    /// \brief Reclaim the memory associated to a symbol.
    /// \param yymsg     Why this token is reclaimed.
    ///                  If null, print nothing.
    /// \param yysym     The symbol.
    template <typename Base>
    void yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const;

  private:
    /// Type access provider for state based symbols.
    struct by_state
    {
      /// Default constructor.
      by_state ();

      /// The symbol type as needed by the constructor.
      typedef state_type kind_type;

      /// Constructor.
      by_state (kind_type s);

      /// Copy constructor.
      by_state (const by_state& other);

      /// Record that this symbol is empty.
      void clear ();

      /// Steal the symbol type from \a that.
      void move (by_state& that);

      /// The (internal) type number (corresponding to \a state).
      /// \a empty_symbol when empty.
      symbol_number_type type_get () const;

      /// The state number used to denote an empty symbol.
      enum { empty_state = -1 };

      /// The state.
      /// \a empty when empty.
      state_type state;
    };

    /// "Internal" symbol: element of the stack.
    struct stack_symbol_type : basic_symbol<by_state>
    {
      /// Superclass.
      typedef basic_symbol<by_state> super_type;
      /// Construct an empty symbol.
      stack_symbol_type ();
      /// Steal the contents from \a sym to build this.
      stack_symbol_type (state_type s, symbol_type& sym);
      /// Assignment, needed by push_back.
      stack_symbol_type& operator= (const stack_symbol_type& that);
    };

    /// Stack type.
    typedef stack<stack_symbol_type> stack_type;

    /// The stack.
    stack_type yystack_;

    /// Push a new state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param s    the symbol
    /// \warning the contents of \a s.value is stolen.
    void yypush_ (const char* m, stack_symbol_type& s);

    /// Push a new look ahead token on the state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param s    the state
    /// \param sym  the symbol (for its value and location).
    /// \warning the contents of \a s.value is stolen.
    void yypush_ (const char* m, state_type s, symbol_type& sym);

    /// Pop \a n symbols the three stacks.
    void yypop_ (unsigned int n = 1);

    /// Constants.
    enum
    {
      yyeof_ = 0,
      yylast_ = 395,     ///< Last index in yytable_.
      yynnts_ = 46,  ///< Number of nonterminal symbols.
      yyfinal_ = 2, ///< Termination state number.
      yyterror_ = 1,
      yyerrcode_ = 256,
      yyntokens_ = 95  ///< Number of tokens.
    };


    // User arguments.
    yaramod::ParserDriver& driver;
  };

  // Symbol number corresponding to token number t.
  inline
   Parser ::token_number_type
   Parser ::yytranslate_ (token_type t)
  {
    static
    const token_number_type
    translate_table[] =
    {
     0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94
    };
    const unsigned int user_token_number_max_ = 349;
    const token_number_type undef_token_ = 2;

    if (static_cast<int>(t) <= yyeof_)
      return yyeof_;
    else if (static_cast<unsigned int> (t) <= user_token_number_max_)
      return translate_table[t];
    else
      return undef_token_;
  }

  inline
   Parser ::syntax_error::syntax_error (const location_type& l, const std::string& m)
    : std::runtime_error (m)
    , location (l)
  {}

  // basic_symbol.
  template <typename Base>
  inline
   Parser ::basic_symbol<Base>::basic_symbol ()
    : value ()
  {}

  template <typename Base>
  inline
   Parser ::basic_symbol<Base>::basic_symbol (const basic_symbol& other)
    : Base (other)
    , value ()
    , location (other.location)
  {
      switch (other.type_get ())
    {
      case 110: // condition
      case 111: // expression
      case 114: // primary_expression
      case 115: // range
      case 116: // for_expression
      case 117: // integer_set
      case 119: // string_set
      case 121: // identifier
        value.copy< Expression::Ptr > (other.value);
        break;

      case 125: // boolean
      case 139: // regexp_greedy
        value.copy< bool > (other.value);
        break;

      case 130: // hex_or
      case 132: // hex_jump
        value.copy< std::shared_ptr<yaramod::HexStringUnit> > (other.value);
        break;

      case 136: // regexp_or
      case 138: // regexp_repeat
      case 140: // regexp_single
        value.copy< std::shared_ptr<yaramod::RegexpUnit> > (other.value);
        break;

      case 106: // strings
      case 107: // strings_body
        value.copy< std::shared_ptr<yaramod::Rule::StringsTrie> > (other.value);
        break;

      case 108: // string
      case 133: // regexp
      case 135: // regexp_body
        value.copy< std::shared_ptr<yaramod::String> > (other.value);
        break;

      case 58: // "/"
      case 59: // "string literal"
      case 60: // "integer"
      case 61: // "float"
      case 62: // "string identifier"
      case 63: // "string wildcard"
      case 64: // "string length"
      case 65: // "string offset"
      case 66: // "string count"
      case 67: // "identifier"
      case 68: // "fixed-width integer function"
      case 91: // "regexp character"
      case 93: // "regexp class"
        value.copy< std::string > (other.value);
        break;

      case 123: // string_mods
        value.copy< std::uint32_t > (other.value);
        break;

      case 75: // "hex string integer"
        value.copy< std::uint64_t > (other.value);
        break;

      case 74: // "hex string nibble"
        value.copy< std::uint8_t > (other.value);
        break;

      case 118: // integer_enumeration
      case 120: // string_enumeration
      case 122: // arguments
        value.copy< std::vector<Expression::Ptr> > (other.value);
        break;

      case 131: // hex_or_body
        value.copy< std::vector<std::shared_ptr<yaramod::HexString>> > (other.value);
        break;

      case 126: // hex_string
      case 127: // hex_string_edge
      case 128: // hex_byte
      case 129: // hex_string_body
        value.copy< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > (other.value);
        break;

      case 137: // regexp_concat
        value.copy< std::vector<std::shared_ptr<yaramod::RegexpUnit>> > (other.value);
        break;

      case 102: // tags
      case 103: // tag_list
        value.copy< std::vector<std::string> > (other.value);
        break;

      case 104: // metas
      case 105: // metas_body
        value.copy< std::vector<yaramod::Meta> > (other.value);
        break;

      case 124: // literal
        value.copy< yaramod::Literal > (other.value);
        break;

      case 92: // "regexp range"
        value.copy< yaramod::RegexpRangePair > (other.value);
        break;

      case 98: // rule
        value.copy< yaramod::Rule > (other.value);
        break;

      case 101: // rule_mod
        value.copy< yaramod::Rule::Modifier > (other.value);
        break;

      default:
        break;
    }

  }


  template <typename Base>
  inline
   Parser ::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const semantic_type& v, const location_type& l)
    : Base (t)
    , value ()
    , location (l)
  {
    (void) v;
      switch (this->type_get ())
    {
      case 110: // condition
      case 111: // expression
      case 114: // primary_expression
      case 115: // range
      case 116: // for_expression
      case 117: // integer_set
      case 119: // string_set
      case 121: // identifier
        value.copy< Expression::Ptr > (v);
        break;

      case 125: // boolean
      case 139: // regexp_greedy
        value.copy< bool > (v);
        break;

      case 130: // hex_or
      case 132: // hex_jump
        value.copy< std::shared_ptr<yaramod::HexStringUnit> > (v);
        break;

      case 136: // regexp_or
      case 138: // regexp_repeat
      case 140: // regexp_single
        value.copy< std::shared_ptr<yaramod::RegexpUnit> > (v);
        break;

      case 106: // strings
      case 107: // strings_body
        value.copy< std::shared_ptr<yaramod::Rule::StringsTrie> > (v);
        break;

      case 108: // string
      case 133: // regexp
      case 135: // regexp_body
        value.copy< std::shared_ptr<yaramod::String> > (v);
        break;

      case 58: // "/"
      case 59: // "string literal"
      case 60: // "integer"
      case 61: // "float"
      case 62: // "string identifier"
      case 63: // "string wildcard"
      case 64: // "string length"
      case 65: // "string offset"
      case 66: // "string count"
      case 67: // "identifier"
      case 68: // "fixed-width integer function"
      case 91: // "regexp character"
      case 93: // "regexp class"
        value.copy< std::string > (v);
        break;

      case 123: // string_mods
        value.copy< std::uint32_t > (v);
        break;

      case 75: // "hex string integer"
        value.copy< std::uint64_t > (v);
        break;

      case 74: // "hex string nibble"
        value.copy< std::uint8_t > (v);
        break;

      case 118: // integer_enumeration
      case 120: // string_enumeration
      case 122: // arguments
        value.copy< std::vector<Expression::Ptr> > (v);
        break;

      case 131: // hex_or_body
        value.copy< std::vector<std::shared_ptr<yaramod::HexString>> > (v);
        break;

      case 126: // hex_string
      case 127: // hex_string_edge
      case 128: // hex_byte
      case 129: // hex_string_body
        value.copy< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > (v);
        break;

      case 137: // regexp_concat
        value.copy< std::vector<std::shared_ptr<yaramod::RegexpUnit>> > (v);
        break;

      case 102: // tags
      case 103: // tag_list
        value.copy< std::vector<std::string> > (v);
        break;

      case 104: // metas
      case 105: // metas_body
        value.copy< std::vector<yaramod::Meta> > (v);
        break;

      case 124: // literal
        value.copy< yaramod::Literal > (v);
        break;

      case 92: // "regexp range"
        value.copy< yaramod::RegexpRangePair > (v);
        break;

      case 98: // rule
        value.copy< yaramod::Rule > (v);
        break;

      case 101: // rule_mod
        value.copy< yaramod::Rule::Modifier > (v);
        break;

      default:
        break;
    }
}


  // Implementation of basic_symbol constructor for each type.

  template <typename Base>
   Parser ::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const location_type& l)
    : Base (t)
    , value ()
    , location (l)
  {}

  template <typename Base>
   Parser ::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const Expression::Ptr v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
   Parser ::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const bool v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
   Parser ::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const std::shared_ptr<yaramod::HexStringUnit> v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
   Parser ::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const std::shared_ptr<yaramod::RegexpUnit> v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
   Parser ::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const std::shared_ptr<yaramod::Rule::StringsTrie> v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
   Parser ::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const std::shared_ptr<yaramod::String> v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
   Parser ::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const std::string v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
   Parser ::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const std::uint32_t v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
   Parser ::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const std::uint64_t v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
   Parser ::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const std::uint8_t v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
   Parser ::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const std::vector<Expression::Ptr> v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
   Parser ::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const std::vector<std::shared_ptr<yaramod::HexString>> v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
   Parser ::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const std::vector<std::shared_ptr<yaramod::HexStringUnit>> v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
   Parser ::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const std::vector<std::shared_ptr<yaramod::RegexpUnit>> v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
   Parser ::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const std::vector<std::string> v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
   Parser ::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const std::vector<yaramod::Meta> v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
   Parser ::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const yaramod::Literal v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
   Parser ::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const yaramod::RegexpRangePair v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
   Parser ::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const yaramod::Rule v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
   Parser ::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const yaramod::Rule::Modifier v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}


  template <typename Base>
  inline
   Parser ::basic_symbol<Base>::~basic_symbol ()
  {
    clear ();
  }

  template <typename Base>
  inline
  void
   Parser ::basic_symbol<Base>::clear ()
  {
    // User destructor.
    symbol_number_type yytype = this->type_get ();
    basic_symbol<Base>& yysym = *this;
    (void) yysym;
    switch (yytype)
    {
   default:
      break;
    }

    // Type destructor.
    switch (yytype)
    {
      case 110: // condition
      case 111: // expression
      case 114: // primary_expression
      case 115: // range
      case 116: // for_expression
      case 117: // integer_set
      case 119: // string_set
      case 121: // identifier
        value.template destroy< Expression::Ptr > ();
        break;

      case 125: // boolean
      case 139: // regexp_greedy
        value.template destroy< bool > ();
        break;

      case 130: // hex_or
      case 132: // hex_jump
        value.template destroy< std::shared_ptr<yaramod::HexStringUnit> > ();
        break;

      case 136: // regexp_or
      case 138: // regexp_repeat
      case 140: // regexp_single
        value.template destroy< std::shared_ptr<yaramod::RegexpUnit> > ();
        break;

      case 106: // strings
      case 107: // strings_body
        value.template destroy< std::shared_ptr<yaramod::Rule::StringsTrie> > ();
        break;

      case 108: // string
      case 133: // regexp
      case 135: // regexp_body
        value.template destroy< std::shared_ptr<yaramod::String> > ();
        break;

      case 58: // "/"
      case 59: // "string literal"
      case 60: // "integer"
      case 61: // "float"
      case 62: // "string identifier"
      case 63: // "string wildcard"
      case 64: // "string length"
      case 65: // "string offset"
      case 66: // "string count"
      case 67: // "identifier"
      case 68: // "fixed-width integer function"
      case 91: // "regexp character"
      case 93: // "regexp class"
        value.template destroy< std::string > ();
        break;

      case 123: // string_mods
        value.template destroy< std::uint32_t > ();
        break;

      case 75: // "hex string integer"
        value.template destroy< std::uint64_t > ();
        break;

      case 74: // "hex string nibble"
        value.template destroy< std::uint8_t > ();
        break;

      case 118: // integer_enumeration
      case 120: // string_enumeration
      case 122: // arguments
        value.template destroy< std::vector<Expression::Ptr> > ();
        break;

      case 131: // hex_or_body
        value.template destroy< std::vector<std::shared_ptr<yaramod::HexString>> > ();
        break;

      case 126: // hex_string
      case 127: // hex_string_edge
      case 128: // hex_byte
      case 129: // hex_string_body
        value.template destroy< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ();
        break;

      case 137: // regexp_concat
        value.template destroy< std::vector<std::shared_ptr<yaramod::RegexpUnit>> > ();
        break;

      case 102: // tags
      case 103: // tag_list
        value.template destroy< std::vector<std::string> > ();
        break;

      case 104: // metas
      case 105: // metas_body
        value.template destroy< std::vector<yaramod::Meta> > ();
        break;

      case 124: // literal
        value.template destroy< yaramod::Literal > ();
        break;

      case 92: // "regexp range"
        value.template destroy< yaramod::RegexpRangePair > ();
        break;

      case 98: // rule
        value.template destroy< yaramod::Rule > ();
        break;

      case 101: // rule_mod
        value.template destroy< yaramod::Rule::Modifier > ();
        break;

      default:
        break;
    }

    Base::clear ();
  }

  template <typename Base>
  inline
  bool
   Parser ::basic_symbol<Base>::empty () const
  {
    return Base::type_get () == empty_symbol;
  }

  template <typename Base>
  inline
  void
   Parser ::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move(s);
      switch (this->type_get ())
    {
      case 110: // condition
      case 111: // expression
      case 114: // primary_expression
      case 115: // range
      case 116: // for_expression
      case 117: // integer_set
      case 119: // string_set
      case 121: // identifier
        value.move< Expression::Ptr > (s.value);
        break;

      case 125: // boolean
      case 139: // regexp_greedy
        value.move< bool > (s.value);
        break;

      case 130: // hex_or
      case 132: // hex_jump
        value.move< std::shared_ptr<yaramod::HexStringUnit> > (s.value);
        break;

      case 136: // regexp_or
      case 138: // regexp_repeat
      case 140: // regexp_single
        value.move< std::shared_ptr<yaramod::RegexpUnit> > (s.value);
        break;

      case 106: // strings
      case 107: // strings_body
        value.move< std::shared_ptr<yaramod::Rule::StringsTrie> > (s.value);
        break;

      case 108: // string
      case 133: // regexp
      case 135: // regexp_body
        value.move< std::shared_ptr<yaramod::String> > (s.value);
        break;

      case 58: // "/"
      case 59: // "string literal"
      case 60: // "integer"
      case 61: // "float"
      case 62: // "string identifier"
      case 63: // "string wildcard"
      case 64: // "string length"
      case 65: // "string offset"
      case 66: // "string count"
      case 67: // "identifier"
      case 68: // "fixed-width integer function"
      case 91: // "regexp character"
      case 93: // "regexp class"
        value.move< std::string > (s.value);
        break;

      case 123: // string_mods
        value.move< std::uint32_t > (s.value);
        break;

      case 75: // "hex string integer"
        value.move< std::uint64_t > (s.value);
        break;

      case 74: // "hex string nibble"
        value.move< std::uint8_t > (s.value);
        break;

      case 118: // integer_enumeration
      case 120: // string_enumeration
      case 122: // arguments
        value.move< std::vector<Expression::Ptr> > (s.value);
        break;

      case 131: // hex_or_body
        value.move< std::vector<std::shared_ptr<yaramod::HexString>> > (s.value);
        break;

      case 126: // hex_string
      case 127: // hex_string_edge
      case 128: // hex_byte
      case 129: // hex_string_body
        value.move< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > (s.value);
        break;

      case 137: // regexp_concat
        value.move< std::vector<std::shared_ptr<yaramod::RegexpUnit>> > (s.value);
        break;

      case 102: // tags
      case 103: // tag_list
        value.move< std::vector<std::string> > (s.value);
        break;

      case 104: // metas
      case 105: // metas_body
        value.move< std::vector<yaramod::Meta> > (s.value);
        break;

      case 124: // literal
        value.move< yaramod::Literal > (s.value);
        break;

      case 92: // "regexp range"
        value.move< yaramod::RegexpRangePair > (s.value);
        break;

      case 98: // rule
        value.move< yaramod::Rule > (s.value);
        break;

      case 101: // rule_mod
        value.move< yaramod::Rule::Modifier > (s.value);
        break;

      default:
        break;
    }

    location = s.location;
  }

  // by_type.
  inline
   Parser ::by_type::by_type ()
    : type (empty_symbol)
  {}

  inline
   Parser ::by_type::by_type (const by_type& other)
    : type (other.type)
  {}

  inline
   Parser ::by_type::by_type (token_type t)
    : type (yytranslate_ (t))
  {}

  inline
  void
   Parser ::by_type::clear ()
  {
    type = empty_symbol;
  }

  inline
  void
   Parser ::by_type::move (by_type& that)
  {
    type = that.type;
    that.clear ();
  }

  inline
  int
   Parser ::by_type::type_get () const
  {
    return type;
  }

  inline
   Parser ::token_type
   Parser ::by_type::token () const
  {
    // YYTOKNUM[NUM] -- (External) token number corresponding to the
    // (internal) symbol number NUM (which must be that of a token).  */
    static
    const unsigned short int
    yytoken_number_[] =
    {
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349
    };
    return static_cast<token_type> (yytoken_number_[type]);
  }
  // Implementation of make_symbol for each symbol type.
   Parser ::symbol_type
   Parser ::make_END (const location_type& l)
  {
    return symbol_type (token::END, l);
  }

   Parser ::symbol_type
   Parser ::make_RANGE (const location_type& l)
  {
    return symbol_type (token::RANGE, l);
  }

   Parser ::symbol_type
   Parser ::make_DOT (const location_type& l)
  {
    return symbol_type (token::DOT, l);
  }

   Parser ::symbol_type
   Parser ::make_LT (const location_type& l)
  {
    return symbol_type (token::LT, l);
  }

   Parser ::symbol_type
   Parser ::make_GT (const location_type& l)
  {
    return symbol_type (token::GT, l);
  }

   Parser ::symbol_type
   Parser ::make_LE (const location_type& l)
  {
    return symbol_type (token::LE, l);
  }

   Parser ::symbol_type
   Parser ::make_GE (const location_type& l)
  {
    return symbol_type (token::GE, l);
  }

   Parser ::symbol_type
   Parser ::make_EQ (const location_type& l)
  {
    return symbol_type (token::EQ, l);
  }

   Parser ::symbol_type
   Parser ::make_NEQ (const location_type& l)
  {
    return symbol_type (token::NEQ, l);
  }

   Parser ::symbol_type
   Parser ::make_SHIFT_LEFT (const location_type& l)
  {
    return symbol_type (token::SHIFT_LEFT, l);
  }

   Parser ::symbol_type
   Parser ::make_SHIFT_RIGHT (const location_type& l)
  {
    return symbol_type (token::SHIFT_RIGHT, l);
  }

   Parser ::symbol_type
   Parser ::make_MINUS (const location_type& l)
  {
    return symbol_type (token::MINUS, l);
  }

   Parser ::symbol_type
   Parser ::make_PLUS (const location_type& l)
  {
    return symbol_type (token::PLUS, l);
  }

   Parser ::symbol_type
   Parser ::make_MULTIPLY (const location_type& l)
  {
    return symbol_type (token::MULTIPLY, l);
  }

   Parser ::symbol_type
   Parser ::make_DIVIDE (const location_type& l)
  {
    return symbol_type (token::DIVIDE, l);
  }

   Parser ::symbol_type
   Parser ::make_MODULO (const location_type& l)
  {
    return symbol_type (token::MODULO, l);
  }

   Parser ::symbol_type
   Parser ::make_BITWISE_XOR (const location_type& l)
  {
    return symbol_type (token::BITWISE_XOR, l);
  }

   Parser ::symbol_type
   Parser ::make_BITWISE_AND (const location_type& l)
  {
    return symbol_type (token::BITWISE_AND, l);
  }

   Parser ::symbol_type
   Parser ::make_BITWISE_OR (const location_type& l)
  {
    return symbol_type (token::BITWISE_OR, l);
  }

   Parser ::symbol_type
   Parser ::make_BITWISE_NOT (const location_type& l)
  {
    return symbol_type (token::BITWISE_NOT, l);
  }

   Parser ::symbol_type
   Parser ::make_LP (const location_type& l)
  {
    return symbol_type (token::LP, l);
  }

   Parser ::symbol_type
   Parser ::make_RP (const location_type& l)
  {
    return symbol_type (token::RP, l);
  }

   Parser ::symbol_type
   Parser ::make_LCB (const location_type& l)
  {
    return symbol_type (token::LCB, l);
  }

   Parser ::symbol_type
   Parser ::make_RCB (const location_type& l)
  {
    return symbol_type (token::RCB, l);
  }

   Parser ::symbol_type
   Parser ::make_ASSIGN (const location_type& l)
  {
    return symbol_type (token::ASSIGN, l);
  }

   Parser ::symbol_type
   Parser ::make_COLON (const location_type& l)
  {
    return symbol_type (token::COLON, l);
  }

   Parser ::symbol_type
   Parser ::make_COMMA (const location_type& l)
  {
    return symbol_type (token::COMMA, l);
  }

   Parser ::symbol_type
   Parser ::make_PRIVATE (const location_type& l)
  {
    return symbol_type (token::PRIVATE, l);
  }

   Parser ::symbol_type
   Parser ::make_GLOBAL (const location_type& l)
  {
    return symbol_type (token::GLOBAL, l);
  }

   Parser ::symbol_type
   Parser ::make_RULE (const location_type& l)
  {
    return symbol_type (token::RULE, l);
  }

   Parser ::symbol_type
   Parser ::make_META (const location_type& l)
  {
    return symbol_type (token::META, l);
  }

   Parser ::symbol_type
   Parser ::make_STRINGS (const location_type& l)
  {
    return symbol_type (token::STRINGS, l);
  }

   Parser ::symbol_type
   Parser ::make_CONDITION (const location_type& l)
  {
    return symbol_type (token::CONDITION, l);
  }

   Parser ::symbol_type
   Parser ::make_ASCII (const location_type& l)
  {
    return symbol_type (token::ASCII, l);
  }

   Parser ::symbol_type
   Parser ::make_NOCASE (const location_type& l)
  {
    return symbol_type (token::NOCASE, l);
  }

   Parser ::symbol_type
   Parser ::make_WIDE (const location_type& l)
  {
    return symbol_type (token::WIDE, l);
  }

   Parser ::symbol_type
   Parser ::make_FULLWORD (const location_type& l)
  {
    return symbol_type (token::FULLWORD, l);
  }

   Parser ::symbol_type
   Parser ::make_XOR (const location_type& l)
  {
    return symbol_type (token::XOR, l);
  }

   Parser ::symbol_type
   Parser ::make_BOOL_TRUE (const location_type& l)
  {
    return symbol_type (token::BOOL_TRUE, l);
  }

   Parser ::symbol_type
   Parser ::make_BOOL_FALSE (const location_type& l)
  {
    return symbol_type (token::BOOL_FALSE, l);
  }

   Parser ::symbol_type
   Parser ::make_IMPORT_MODULE (const location_type& l)
  {
    return symbol_type (token::IMPORT_MODULE, l);
  }

   Parser ::symbol_type
   Parser ::make_NOT (const location_type& l)
  {
    return symbol_type (token::NOT, l);
  }

   Parser ::symbol_type
   Parser ::make_AND (const location_type& l)
  {
    return symbol_type (token::AND, l);
  }

   Parser ::symbol_type
   Parser ::make_OR (const location_type& l)
  {
    return symbol_type (token::OR, l);
  }

   Parser ::symbol_type
   Parser ::make_ALL (const location_type& l)
  {
    return symbol_type (token::ALL, l);
  }

   Parser ::symbol_type
   Parser ::make_ANY (const location_type& l)
  {
    return symbol_type (token::ANY, l);
  }

   Parser ::symbol_type
   Parser ::make_OF (const location_type& l)
  {
    return symbol_type (token::OF, l);
  }

   Parser ::symbol_type
   Parser ::make_THEM (const location_type& l)
  {
    return symbol_type (token::THEM, l);
  }

   Parser ::symbol_type
   Parser ::make_FOR (const location_type& l)
  {
    return symbol_type (token::FOR, l);
  }

   Parser ::symbol_type
   Parser ::make_ENTRYPOINT (const location_type& l)
  {
    return symbol_type (token::ENTRYPOINT, l);
  }

   Parser ::symbol_type
   Parser ::make_OP_AT (const location_type& l)
  {
    return symbol_type (token::OP_AT, l);
  }

   Parser ::symbol_type
   Parser ::make_OP_IN (const location_type& l)
  {
    return symbol_type (token::OP_IN, l);
  }

   Parser ::symbol_type
   Parser ::make_FILESIZE (const location_type& l)
  {
    return symbol_type (token::FILESIZE, l);
  }

   Parser ::symbol_type
   Parser ::make_CONTAINS (const location_type& l)
  {
    return symbol_type (token::CONTAINS, l);
  }

   Parser ::symbol_type
   Parser ::make_MATCHES (const location_type& l)
  {
    return symbol_type (token::MATCHES, l);
  }

   Parser ::symbol_type
   Parser ::make_SLASH (const std::string& v, const location_type& l)
  {
    return symbol_type (token::SLASH, v, l);
  }

   Parser ::symbol_type
   Parser ::make_STRING_LITERAL (const std::string& v, const location_type& l)
  {
    return symbol_type (token::STRING_LITERAL, v, l);
  }

   Parser ::symbol_type
   Parser ::make_INTEGER (const std::string& v, const location_type& l)
  {
    return symbol_type (token::INTEGER, v, l);
  }

   Parser ::symbol_type
   Parser ::make_DOUBLE (const std::string& v, const location_type& l)
  {
    return symbol_type (token::DOUBLE, v, l);
  }

   Parser ::symbol_type
   Parser ::make_STRING_ID (const std::string& v, const location_type& l)
  {
    return symbol_type (token::STRING_ID, v, l);
  }

   Parser ::symbol_type
   Parser ::make_STRING_ID_WILDCARD (const std::string& v, const location_type& l)
  {
    return symbol_type (token::STRING_ID_WILDCARD, v, l);
  }

   Parser ::symbol_type
   Parser ::make_STRING_LENGTH (const std::string& v, const location_type& l)
  {
    return symbol_type (token::STRING_LENGTH, v, l);
  }

   Parser ::symbol_type
   Parser ::make_STRING_OFFSET (const std::string& v, const location_type& l)
  {
    return symbol_type (token::STRING_OFFSET, v, l);
  }

   Parser ::symbol_type
   Parser ::make_STRING_COUNT (const std::string& v, const location_type& l)
  {
    return symbol_type (token::STRING_COUNT, v, l);
  }

   Parser ::symbol_type
   Parser ::make_ID (const std::string& v, const location_type& l)
  {
    return symbol_type (token::ID, v, l);
  }

   Parser ::symbol_type
   Parser ::make_INTEGER_FUNCTION (const std::string& v, const location_type& l)
  {
    return symbol_type (token::INTEGER_FUNCTION, v, l);
  }

   Parser ::symbol_type
   Parser ::make_HEX_OR (const location_type& l)
  {
    return symbol_type (token::HEX_OR, l);
  }

   Parser ::symbol_type
   Parser ::make_LSQB (const location_type& l)
  {
    return symbol_type (token::LSQB, l);
  }

   Parser ::symbol_type
   Parser ::make_RSQB (const location_type& l)
  {
    return symbol_type (token::RSQB, l);
  }

   Parser ::symbol_type
   Parser ::make_HEX_WILDCARD (const location_type& l)
  {
    return symbol_type (token::HEX_WILDCARD, l);
  }

   Parser ::symbol_type
   Parser ::make_DASH (const location_type& l)
  {
    return symbol_type (token::DASH, l);
  }

   Parser ::symbol_type
   Parser ::make_HEX_NIBBLE (const std::uint8_t& v, const location_type& l)
  {
    return symbol_type (token::HEX_NIBBLE, v, l);
  }

   Parser ::symbol_type
   Parser ::make_HEX_INTEGER (const std::uint64_t& v, const location_type& l)
  {
    return symbol_type (token::HEX_INTEGER, v, l);
  }

   Parser ::symbol_type
   Parser ::make_REGEXP_OR (const location_type& l)
  {
    return symbol_type (token::REGEXP_OR, l);
  }

   Parser ::symbol_type
   Parser ::make_REGEXP_ITER (const location_type& l)
  {
    return symbol_type (token::REGEXP_ITER, l);
  }

   Parser ::symbol_type
   Parser ::make_REGEXP_PITER (const location_type& l)
  {
    return symbol_type (token::REGEXP_PITER, l);
  }

   Parser ::symbol_type
   Parser ::make_REGEXP_OPTIONAL (const location_type& l)
  {
    return symbol_type (token::REGEXP_OPTIONAL, l);
  }

   Parser ::symbol_type
   Parser ::make_REGEXP_START_OF_LINE (const location_type& l)
  {
    return symbol_type (token::REGEXP_START_OF_LINE, l);
  }

   Parser ::symbol_type
   Parser ::make_REGEXP_END_OF_LINE (const location_type& l)
  {
    return symbol_type (token::REGEXP_END_OF_LINE, l);
  }

   Parser ::symbol_type
   Parser ::make_REGEXP_ANY_CHAR (const location_type& l)
  {
    return symbol_type (token::REGEXP_ANY_CHAR, l);
  }

   Parser ::symbol_type
   Parser ::make_REGEXP_WORD_CHAR (const location_type& l)
  {
    return symbol_type (token::REGEXP_WORD_CHAR, l);
  }

   Parser ::symbol_type
   Parser ::make_REGEXP_NON_WORD_CHAR (const location_type& l)
  {
    return symbol_type (token::REGEXP_NON_WORD_CHAR, l);
  }

   Parser ::symbol_type
   Parser ::make_REGEXP_SPACE (const location_type& l)
  {
    return symbol_type (token::REGEXP_SPACE, l);
  }

   Parser ::symbol_type
   Parser ::make_REGEXP_NON_SPACE (const location_type& l)
  {
    return symbol_type (token::REGEXP_NON_SPACE, l);
  }

   Parser ::symbol_type
   Parser ::make_REGEXP_DIGIT (const location_type& l)
  {
    return symbol_type (token::REGEXP_DIGIT, l);
  }

   Parser ::symbol_type
   Parser ::make_REGEXP_NON_DIGIT (const location_type& l)
  {
    return symbol_type (token::REGEXP_NON_DIGIT, l);
  }

   Parser ::symbol_type
   Parser ::make_REGEXP_WORD_BOUNDARY (const location_type& l)
  {
    return symbol_type (token::REGEXP_WORD_BOUNDARY, l);
  }

   Parser ::symbol_type
   Parser ::make_REGEXP_NON_WORD_BOUNDARY (const location_type& l)
  {
    return symbol_type (token::REGEXP_NON_WORD_BOUNDARY, l);
  }

   Parser ::symbol_type
   Parser ::make_REGEXP_CHAR (const std::string& v, const location_type& l)
  {
    return symbol_type (token::REGEXP_CHAR, v, l);
  }

   Parser ::symbol_type
   Parser ::make_REGEXP_RANGE (const yaramod::RegexpRangePair& v, const location_type& l)
  {
    return symbol_type (token::REGEXP_RANGE, v, l);
  }

   Parser ::symbol_type
   Parser ::make_REGEXP_CLASS (const std::string& v, const location_type& l)
  {
    return symbol_type (token::REGEXP_CLASS, v, l);
  }

   Parser ::symbol_type
   Parser ::make_UNARY_MINUS (const location_type& l)
  {
    return symbol_type (token::UNARY_MINUS, l);
  }


#line 61 "parser/yy/parser.y" // lalr1.cc:377
} } //  yaramod::yy 
#line 2633 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.hpp" // lalr1.cc:377




#endif // !YY_YY_HOME_SLOAN_DISPATCH_LINUX_RETDEC_BUILD_EXTERNAL_SRC_YARAMOD_PROJECT_BUILD_SRC_YARAMOD_YY_YY_PARSER_HPP_INCLUDED
