// A Bison parser, made by GNU Bison 3.0.4.

// Skeleton implementation for Bison LALR(1) parsers in C++

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
// //                    "%code top" blocks.
#line 41 "parser/yy/parser.y" // lalr1.cc:397

#include "yaramod/parser/parser_driver.h"

using namespace yaramod;

static yy::Parser::symbol_type yylex(ParserDriver& driver)
{
	return driver.getLexer().getNextToken();
}

#line 45 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:397


// First part of user declarations.

#line 50 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:404

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

#include "yy_parser.hpp"

// User implementation prologue.

#line 64 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:412


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (/*CONSTCOND*/ false)
# endif


// Suppress unused-variable warnings by "using" E.
#define YYUSE(E) ((void) (E))

// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << std::endl;                  \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yystack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE(Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void>(0)
# define YY_STACK_PRINT()                static_cast<void>(0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 61 "parser/yy/parser.y" // lalr1.cc:479
namespace  yaramod { namespace yy  {
#line 150 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:479

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
   Parser ::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr = "";
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              // Fall through.
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }


  /// Build a parser object.
   Parser :: Parser  (yaramod::ParserDriver& driver_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      driver (driver_yyarg)
  {}

   Parser ::~ Parser  ()
  {}


  /*---------------.
  | Symbol types.  |
  `---------------*/



  // by_state.
  inline
   Parser ::by_state::by_state ()
    : state (empty_state)
  {}

  inline
   Parser ::by_state::by_state (const by_state& other)
    : state (other.state)
  {}

  inline
  void
   Parser ::by_state::clear ()
  {
    state = empty_state;
  }

  inline
  void
   Parser ::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  inline
   Parser ::by_state::by_state (state_type s)
    : state (s)
  {}

  inline
   Parser ::symbol_number_type
   Parser ::by_state::type_get () const
  {
    if (state == empty_state)
      return empty_symbol;
    else
      return yystos_[state];
  }

  inline
   Parser ::stack_symbol_type::stack_symbol_type ()
  {}


  inline
   Parser ::stack_symbol_type::stack_symbol_type (state_type s, symbol_type& that)
    : super_type (s, that.location)
  {
      switch (that.type_get ())
    {
      case 110: // condition
      case 111: // expression
      case 114: // primary_expression
      case 115: // range
      case 116: // for_expression
      case 117: // integer_set
      case 119: // string_set
      case 121: // identifier
        value.move< Expression::Ptr > (that.value);
        break;

      case 125: // boolean
      case 139: // regexp_greedy
        value.move< bool > (that.value);
        break;

      case 130: // hex_or
      case 132: // hex_jump
        value.move< std::shared_ptr<yaramod::HexStringUnit> > (that.value);
        break;

      case 136: // regexp_or
      case 138: // regexp_repeat
      case 140: // regexp_single
        value.move< std::shared_ptr<yaramod::RegexpUnit> > (that.value);
        break;

      case 106: // strings
      case 107: // strings_body
        value.move< std::shared_ptr<yaramod::Rule::StringsTrie> > (that.value);
        break;

      case 108: // string
      case 133: // regexp
      case 135: // regexp_body
        value.move< std::shared_ptr<yaramod::String> > (that.value);
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
        value.move< std::string > (that.value);
        break;

      case 123: // string_mods
        value.move< std::uint32_t > (that.value);
        break;

      case 75: // "hex string integer"
        value.move< std::uint64_t > (that.value);
        break;

      case 74: // "hex string nibble"
        value.move< std::uint8_t > (that.value);
        break;

      case 118: // integer_enumeration
      case 120: // string_enumeration
      case 122: // arguments
        value.move< std::vector<Expression::Ptr> > (that.value);
        break;

      case 131: // hex_or_body
        value.move< std::vector<std::shared_ptr<yaramod::HexString>> > (that.value);
        break;

      case 126: // hex_string
      case 127: // hex_string_edge
      case 128: // hex_byte
      case 129: // hex_string_body
        value.move< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > (that.value);
        break;

      case 137: // regexp_concat
        value.move< std::vector<std::shared_ptr<yaramod::RegexpUnit>> > (that.value);
        break;

      case 102: // tags
      case 103: // tag_list
        value.move< std::vector<std::string> > (that.value);
        break;

      case 104: // metas
      case 105: // metas_body
        value.move< std::vector<yaramod::Meta> > (that.value);
        break;

      case 124: // literal
        value.move< yaramod::Literal > (that.value);
        break;

      case 92: // "regexp range"
        value.move< yaramod::RegexpRangePair > (that.value);
        break;

      case 98: // rule
        value.move< yaramod::Rule > (that.value);
        break;

      case 101: // rule_mod
        value.move< yaramod::Rule::Modifier > (that.value);
        break;

      default:
        break;
    }

    // that is emptied.
    that.type = empty_symbol;
  }

  inline
   Parser ::stack_symbol_type&
   Parser ::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
      switch (that.type_get ())
    {
      case 110: // condition
      case 111: // expression
      case 114: // primary_expression
      case 115: // range
      case 116: // for_expression
      case 117: // integer_set
      case 119: // string_set
      case 121: // identifier
        value.copy< Expression::Ptr > (that.value);
        break;

      case 125: // boolean
      case 139: // regexp_greedy
        value.copy< bool > (that.value);
        break;

      case 130: // hex_or
      case 132: // hex_jump
        value.copy< std::shared_ptr<yaramod::HexStringUnit> > (that.value);
        break;

      case 136: // regexp_or
      case 138: // regexp_repeat
      case 140: // regexp_single
        value.copy< std::shared_ptr<yaramod::RegexpUnit> > (that.value);
        break;

      case 106: // strings
      case 107: // strings_body
        value.copy< std::shared_ptr<yaramod::Rule::StringsTrie> > (that.value);
        break;

      case 108: // string
      case 133: // regexp
      case 135: // regexp_body
        value.copy< std::shared_ptr<yaramod::String> > (that.value);
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
        value.copy< std::string > (that.value);
        break;

      case 123: // string_mods
        value.copy< std::uint32_t > (that.value);
        break;

      case 75: // "hex string integer"
        value.copy< std::uint64_t > (that.value);
        break;

      case 74: // "hex string nibble"
        value.copy< std::uint8_t > (that.value);
        break;

      case 118: // integer_enumeration
      case 120: // string_enumeration
      case 122: // arguments
        value.copy< std::vector<Expression::Ptr> > (that.value);
        break;

      case 131: // hex_or_body
        value.copy< std::vector<std::shared_ptr<yaramod::HexString>> > (that.value);
        break;

      case 126: // hex_string
      case 127: // hex_string_edge
      case 128: // hex_byte
      case 129: // hex_string_body
        value.copy< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > (that.value);
        break;

      case 137: // regexp_concat
        value.copy< std::vector<std::shared_ptr<yaramod::RegexpUnit>> > (that.value);
        break;

      case 102: // tags
      case 103: // tag_list
        value.copy< std::vector<std::string> > (that.value);
        break;

      case 104: // metas
      case 105: // metas_body
        value.copy< std::vector<yaramod::Meta> > (that.value);
        break;

      case 124: // literal
        value.copy< yaramod::Literal > (that.value);
        break;

      case 92: // "regexp range"
        value.copy< yaramod::RegexpRangePair > (that.value);
        break;

      case 98: // rule
        value.copy< yaramod::Rule > (that.value);
        break;

      case 101: // rule_mod
        value.copy< yaramod::Rule::Modifier > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    return *this;
  }


  template <typename Base>
  inline
  void
   Parser ::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
   Parser ::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
    // Avoid a (spurious) G++ 4.8 warning about "array subscript is
    // below array bounds".
    if (yysym.empty ())
      std::abort ();
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " ("
        << yysym.location << ": ";
    YYUSE (yytype);
    yyo << ')';
  }
#endif

  inline
  void
   Parser ::yypush_ (const char* m, state_type s, symbol_type& sym)
  {
    stack_symbol_type t (s, sym);
    yypush_ (m, t);
  }

  inline
  void
   Parser ::yypush_ (const char* m, stack_symbol_type& s)
  {
    if (m)
      YY_SYMBOL_PRINT (m, s);
    yystack_.push (s);
  }

  inline
  void
   Parser ::yypop_ (unsigned int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
   Parser ::debug_stream () const
  {
    return *yycdebug_;
  }

  void
   Parser ::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


   Parser ::debug_level_type
   Parser ::debug_level () const
  {
    return yydebug_;
  }

  void
   Parser ::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  inline  Parser ::state_type
   Parser ::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  inline bool
   Parser ::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
   Parser ::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
   Parser ::parse ()
  {
    // State.
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

    // FIXME: This shoud be completely indented.  It is not yet to
    // avoid gratuitous conflicts when merging into the master branch.
    try
      {
    YYCDEBUG << "Starting parse" << std::endl;


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, yyla);

    // A new symbol was pushed on the stack.
  yynewstate:
    YYCDEBUG << "Entering state " << yystack_[0].state << std::endl;

    // Accept?
    if (yystack_[0].state == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    // Backup.
  yybackup:

    // Try to take a decision without lookahead.
    yyn = yypact_[yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token: ";
        try
          {
            symbol_type yylookahead (yylex (driver));
            yyla.move (yylookahead);
          }
        catch (const syntax_error& yyexc)
          {
            error (yyexc);
            goto yyerrlab1;
          }
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      goto yydefault;

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", yyn, yyla);
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_(yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
        switch (yyr1_[yyn])
    {
      case 110: // condition
      case 111: // expression
      case 114: // primary_expression
      case 115: // range
      case 116: // for_expression
      case 117: // integer_set
      case 119: // string_set
      case 121: // identifier
        yylhs.value.build< Expression::Ptr > ();
        break;

      case 125: // boolean
      case 139: // regexp_greedy
        yylhs.value.build< bool > ();
        break;

      case 130: // hex_or
      case 132: // hex_jump
        yylhs.value.build< std::shared_ptr<yaramod::HexStringUnit> > ();
        break;

      case 136: // regexp_or
      case 138: // regexp_repeat
      case 140: // regexp_single
        yylhs.value.build< std::shared_ptr<yaramod::RegexpUnit> > ();
        break;

      case 106: // strings
      case 107: // strings_body
        yylhs.value.build< std::shared_ptr<yaramod::Rule::StringsTrie> > ();
        break;

      case 108: // string
      case 133: // regexp
      case 135: // regexp_body
        yylhs.value.build< std::shared_ptr<yaramod::String> > ();
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
        yylhs.value.build< std::string > ();
        break;

      case 123: // string_mods
        yylhs.value.build< std::uint32_t > ();
        break;

      case 75: // "hex string integer"
        yylhs.value.build< std::uint64_t > ();
        break;

      case 74: // "hex string nibble"
        yylhs.value.build< std::uint8_t > ();
        break;

      case 118: // integer_enumeration
      case 120: // string_enumeration
      case 122: // arguments
        yylhs.value.build< std::vector<Expression::Ptr> > ();
        break;

      case 131: // hex_or_body
        yylhs.value.build< std::vector<std::shared_ptr<yaramod::HexString>> > ();
        break;

      case 126: // hex_string
      case 127: // hex_string_edge
      case 128: // hex_byte
      case 129: // hex_string_body
        yylhs.value.build< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ();
        break;

      case 137: // regexp_concat
        yylhs.value.build< std::vector<std::shared_ptr<yaramod::RegexpUnit>> > ();
        break;

      case 102: // tags
      case 103: // tag_list
        yylhs.value.build< std::vector<std::string> > ();
        break;

      case 104: // metas
      case 105: // metas_body
        yylhs.value.build< std::vector<yaramod::Meta> > ();
        break;

      case 124: // literal
        yylhs.value.build< yaramod::Literal > ();
        break;

      case 92: // "regexp range"
        yylhs.value.build< yaramod::RegexpRangePair > ();
        break;

      case 98: // rule
        yylhs.value.build< yaramod::Rule > ();
        break;

      case 101: // rule_mod
        yylhs.value.build< yaramod::Rule::Modifier > ();
        break;

      default:
        break;
    }


      // Compute the default @$.
      {
        slice<stack_symbol_type, stack_type> slice (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, slice, yylen);
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
      try
        {
          switch (yyn)
            {
  case 4:
#line 219 "parser/yy/parser.y" // lalr1.cc:859
    { YYACCEPT; }
#line 864 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 6:
#line 225 "parser/yy/parser.y" // lalr1.cc:859
    {
			if (!driver._file.addImport(yystack_[0].value.as< std::string > ()))
			{
				error(driver.getLocation(), "Unrecognized module '" + yystack_[0].value.as< std::string > () + "' imported");
				YYABORT;
			}
		}
#line 876 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 7:
#line 236 "parser/yy/parser.y" // lalr1.cc:859
    {
			driver.markStartOfRule();
		}
#line 884 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 8:
#line 240 "parser/yy/parser.y" // lalr1.cc:859
    {
			if (driver.ruleExists(yystack_[0].value.as< std::string > ()))
			{
				error(driver.getLocation(), "Redefinition of rule '" + yystack_[0].value.as< std::string > () + "'");
				YYABORT;
			}
		}
#line 896 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 9:
#line 248 "parser/yy/parser.y" // lalr1.cc:859
    {
			driver.addRule(Rule(std::move(yystack_[7].value.as< std::string > ()), yystack_[10].value.as< yaramod::Rule::Modifier > (), std::move(yystack_[3].value.as< std::vector<yaramod::Meta> > ()), std::move(yystack_[2].value.as< std::shared_ptr<yaramod::Rule::StringsTrie> > ()), std::move(yystack_[1].value.as< Expression::Ptr > ()), std::move(yystack_[5].value.as< std::vector<std::string> > ())));
		}
#line 904 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 10:
#line 254 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< yaramod::Rule::Modifier > () = Rule::Modifier::Global; }
#line 910 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 11:
#line 255 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< yaramod::Rule::Modifier > () = Rule::Modifier::Private; }
#line 916 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 12:
#line 256 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< yaramod::Rule::Modifier > () = Rule::Modifier::None; }
#line 922 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 13:
#line 260 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::vector<std::string> > () = std::move(yystack_[0].value.as< std::vector<std::string> > ()); }
#line 928 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 14:
#line 261 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::vector<std::string> > ().clear(); }
#line 934 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 15:
#line 266 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< std::vector<std::string> > () = std::move(yystack_[1].value.as< std::vector<std::string> > ());
			yylhs.value.as< std::vector<std::string> > ().push_back(std::move(yystack_[0].value.as< std::string > ()));
		}
#line 943 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 16:
#line 270 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::vector<std::string> > ().push_back(std::move(yystack_[0].value.as< std::string > ())); }
#line 949 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 17:
#line 274 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::vector<yaramod::Meta> > () = std::move(yystack_[0].value.as< std::vector<yaramod::Meta> > ()); }
#line 955 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 18:
#line 275 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::vector<yaramod::Meta> > ().clear(); }
#line 961 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 19:
#line 280 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< std::vector<yaramod::Meta> > () = std::move(yystack_[3].value.as< std::vector<yaramod::Meta> > ());
			yylhs.value.as< std::vector<yaramod::Meta> > ().emplace_back(std::move(yystack_[2].value.as< std::string > ()), std::move(yystack_[0].value.as< yaramod::Literal > ()));
		}
#line 970 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 20:
#line 284 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::vector<yaramod::Meta> > ().clear(); }
#line 976 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 21:
#line 288 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<yaramod::Rule::StringsTrie> > () = std::move(yystack_[0].value.as< std::shared_ptr<yaramod::Rule::StringsTrie> > ()); }
#line 982 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 22:
#line 290 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< std::shared_ptr<yaramod::Rule::StringsTrie> > () = std::make_shared<Rule::StringsTrie>();
			driver.setCurrentStrings(yylhs.value.as< std::shared_ptr<yaramod::Rule::StringsTrie> > ());
		}
#line 991 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 23:
#line 298 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< std::shared_ptr<yaramod::Rule::StringsTrie> > () = std::move(yystack_[3].value.as< std::shared_ptr<yaramod::Rule::StringsTrie> > ());
			yystack_[0].value.as< std::shared_ptr<yaramod::String> > ()->setIdentifier(std::move(yystack_[2].value.as< std::string > ()));

			if (!yylhs.value.as< std::shared_ptr<yaramod::Rule::StringsTrie> > ()->insert(yystack_[0].value.as< std::shared_ptr<yaramod::String> > ()->getIdentifier(), std::move(yystack_[0].value.as< std::shared_ptr<yaramod::String> > ())))
			{
				error(driver.getLocation(), "Redefinition of string '" + yystack_[0].value.as< std::shared_ptr<yaramod::String> > ()->getIdentifier() + "'");
				YYABORT;
			}
		}
#line 1006 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 24:
#line 309 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< std::shared_ptr<yaramod::Rule::StringsTrie> > () = std::make_shared<Rule::StringsTrie>();
			driver.setCurrentStrings(yylhs.value.as< std::shared_ptr<yaramod::Rule::StringsTrie> > ());
		}
#line 1015 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 25:
#line 317 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< std::shared_ptr<yaramod::String> > () = std::make_shared<PlainString>(std::move(yystack_[1].value.as< std::string > ()));
			yylhs.value.as< std::shared_ptr<yaramod::String> > ()->setModifiers(yystack_[0].value.as< std::uint32_t > ());
		}
#line 1024 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 26:
#line 322 "parser/yy/parser.y" // lalr1.cc:859
    {
			driver.getLexer().switchToHexLexer();
		}
#line 1032 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 27:
#line 326 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< std::shared_ptr<yaramod::String> > () = std::make_shared<HexString>(std::move(yystack_[1].value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ()));
			driver.getLexer().switchToYaraLexer();
		}
#line 1041 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 28:
#line 331 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< std::shared_ptr<yaramod::String> > () = std::move(yystack_[1].value.as< std::shared_ptr<yaramod::String> > ());
			yylhs.value.as< std::shared_ptr<yaramod::String> > ()->setModifiers(yystack_[0].value.as< std::uint32_t > ());
		}
#line 1050 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 29:
#line 338 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< Expression::Ptr > () = std::move(yystack_[0].value.as< Expression::Ptr > ()); }
#line 1056 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 30:
#line 343 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< Expression::Ptr > () = std::make_shared<BoolLiteralExpression>(yystack_[0].value.as< bool > ());
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Bool);
		}
#line 1065 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 31:
#line 348 "parser/yy/parser.y" // lalr1.cc:859
    {
			if (!driver.stringExists(yystack_[0].value.as< std::string > ()))
			{
				error(driver.getLocation(), "Reference to undefined string '" + yystack_[0].value.as< std::string > () + "'");
				YYABORT;
			}

			yylhs.value.as< Expression::Ptr > () = std::make_shared<StringExpression>(std::move(yystack_[0].value.as< std::string > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Bool);
		}
#line 1080 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 32:
#line 359 "parser/yy/parser.y" // lalr1.cc:859
    {
			if (!driver.stringExists(yystack_[2].value.as< std::string > ()))
			{
				error(driver.getLocation(), "Reference to undefined string '" + yystack_[2].value.as< std::string > () + "'");
				YYABORT;
			}

			if (!yystack_[0].value.as< Expression::Ptr > ()->isInt())
			{
				error(driver.getLocation(), "operator 'at' expects integer on the right-hand side of the expression");
				YYABORT;
			}

			yylhs.value.as< Expression::Ptr > () = std::make_shared<StringAtExpression>(std::move(yystack_[2].value.as< std::string > ()), std::move(yystack_[0].value.as< Expression::Ptr > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Bool);
		}
#line 1101 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 33:
#line 376 "parser/yy/parser.y" // lalr1.cc:859
    {
			if (!driver.stringExists(yystack_[2].value.as< std::string > ()))
			{
				error(driver.getLocation(), "Reference to undefined string '" + yystack_[2].value.as< std::string > () + "'");
				YYABORT;
			}

			yylhs.value.as< Expression::Ptr > () = std::make_shared<StringInRangeExpression>(std::move(yystack_[2].value.as< std::string > ()), std::move(yystack_[0].value.as< Expression::Ptr > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Bool);
		}
#line 1116 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 34:
#line 387 "parser/yy/parser.y" // lalr1.cc:859
    {
			auto symbol = std::make_shared<ValueSymbol>(yystack_[0].value.as< std::string > (), Expression::Type::Int);
			if (!driver.addLocalSymbol(symbol))
			{
				error(driver.getLocation(), "Redefinition of identifier '" + yystack_[0].value.as< std::string > () + "'");
				YYABORT;
			}
		}
#line 1129 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 35:
#line 396 "parser/yy/parser.y" // lalr1.cc:859
    {
			/* Delete $id before we move it to ForIntExpression */
			driver.removeLocalSymbol(yystack_[7].value.as< std::string > ());
			yylhs.value.as< Expression::Ptr > () = std::make_shared<ForIntExpression>(std::move(yystack_[8].value.as< Expression::Ptr > ()), std::move(yystack_[7].value.as< std::string > ()), std::move(yystack_[4].value.as< Expression::Ptr > ()), std::move(yystack_[1].value.as< Expression::Ptr > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Bool);
		}
#line 1140 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 36:
#line 403 "parser/yy/parser.y" // lalr1.cc:859
    {
			if (driver.isInStringLoop())
			{
				error(driver.getLocation(), "Nesting of for-loop over strings is not allowed");
				YYABORT;
			}

			driver.stringLoopEnter();
		}
#line 1154 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 37:
#line 413 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< Expression::Ptr > () = std::make_shared<ForStringExpression>(std::move(yystack_[7].value.as< Expression::Ptr > ()), std::move(yystack_[5].value.as< Expression::Ptr > ()), std::move(yystack_[1].value.as< Expression::Ptr > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Bool);

			driver.stringLoopLeave();
		}
#line 1165 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 38:
#line 420 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< Expression::Ptr > () = std::make_shared<OfExpression>(std::move(yystack_[2].value.as< Expression::Ptr > ()), std::move(yystack_[0].value.as< Expression::Ptr > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Bool);
		}
#line 1174 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 39:
#line 425 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< Expression::Ptr > () = std::make_shared<NotExpression>(std::move(yystack_[0].value.as< Expression::Ptr > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Bool);
		}
#line 1183 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 40:
#line 430 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< Expression::Ptr > () = std::make_shared<AndExpression>(std::move(yystack_[2].value.as< Expression::Ptr > ()), std::move(yystack_[0].value.as< Expression::Ptr > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Bool);
		}
#line 1192 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 41:
#line 435 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< Expression::Ptr > () = std::make_shared<OrExpression>(std::move(yystack_[2].value.as< Expression::Ptr > ()), std::move(yystack_[0].value.as< Expression::Ptr > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Bool);
		}
#line 1201 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 42:
#line 440 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< Expression::Ptr > () = std::make_shared<LtExpression>(std::move(yystack_[2].value.as< Expression::Ptr > ()), std::move(yystack_[0].value.as< Expression::Ptr > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Bool);
		}
#line 1210 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 43:
#line 445 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< Expression::Ptr > () = std::make_shared<GtExpression>(std::move(yystack_[2].value.as< Expression::Ptr > ()), std::move(yystack_[0].value.as< Expression::Ptr > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Bool);
		}
#line 1219 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 44:
#line 450 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< Expression::Ptr > () = std::make_shared<LeExpression>(std::move(yystack_[2].value.as< Expression::Ptr > ()), std::move(yystack_[0].value.as< Expression::Ptr > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Bool);
		}
#line 1228 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 45:
#line 455 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< Expression::Ptr > () = std::make_shared<GeExpression>(std::move(yystack_[2].value.as< Expression::Ptr > ()), std::move(yystack_[0].value.as< Expression::Ptr > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Bool);
		}
#line 1237 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 46:
#line 460 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< Expression::Ptr > () = std::make_shared<EqExpression>(std::move(yystack_[2].value.as< Expression::Ptr > ()), std::move(yystack_[0].value.as< Expression::Ptr > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Bool);
		}
#line 1246 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 47:
#line 465 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< Expression::Ptr > () = std::make_shared<NeqExpression>(std::move(yystack_[2].value.as< Expression::Ptr > ()), std::move(yystack_[0].value.as< Expression::Ptr > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Bool);
		}
#line 1255 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 48:
#line 470 "parser/yy/parser.y" // lalr1.cc:859
    {
			if (!yystack_[2].value.as< Expression::Ptr > ()->isString())
			{
				error(driver.getLocation(), "operator 'contains' expects string on the left-hand side of the expression");
				YYABORT;
			}

			if (!yystack_[0].value.as< Expression::Ptr > ()->isString())
			{
				error(driver.getLocation(), "operator 'contains' expects string on the right-hand side of the expression");
				YYABORT;
			}

			yylhs.value.as< Expression::Ptr > () = std::make_shared<ContainsExpression>(std::move(yystack_[2].value.as< Expression::Ptr > ()), std::move(yystack_[0].value.as< Expression::Ptr > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Bool);
		}
#line 1276 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 49:
#line 487 "parser/yy/parser.y" // lalr1.cc:859
    {
			if (!yystack_[2].value.as< Expression::Ptr > ()->isString())
			{
				error(driver.getLocation(), "operator 'matches' expects string on the left-hand side of the expression");
				YYABORT;
			}

			auto regexp = std::make_shared<RegexpExpression>(std::move(yystack_[0].value.as< std::shared_ptr<yaramod::String> > ()));
			yylhs.value.as< Expression::Ptr > () = std::make_shared<MatchesExpression>(std::move(yystack_[2].value.as< Expression::Ptr > ()), std::move(regexp));
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Bool);
		}
#line 1292 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 50:
#line 499 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< Expression::Ptr > () = std::move(yystack_[0].value.as< Expression::Ptr > ());
		}
#line 1300 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 51:
#line 503 "parser/yy/parser.y" // lalr1.cc:859
    {
			auto type = yystack_[1].value.as< Expression::Ptr > ()->getType();
			yylhs.value.as< Expression::Ptr > () = std::make_shared<ParenthesesExpression>(std::move(yystack_[1].value.as< Expression::Ptr > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(type);
		}
#line 1310 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 52:
#line 512 "parser/yy/parser.y" // lalr1.cc:859
    {
			auto type = yystack_[1].value.as< Expression::Ptr > ()->getType();
			yylhs.value.as< Expression::Ptr > () = std::make_shared<ParenthesesExpression>(std::move(yystack_[1].value.as< Expression::Ptr > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(type);
		}
#line 1320 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 53:
#line 518 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< Expression::Ptr > () = std::make_shared<FilesizeExpression>();
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Int);
		}
#line 1329 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 54:
#line 523 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< Expression::Ptr > () = std::make_shared<EntrypointExpression>();
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Int);
		}
#line 1338 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 55:
#line 528 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< Expression::Ptr > () = std::make_shared<IntLiteralExpression>(std::move(yystack_[0].value.as< std::string > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Int);
		}
#line 1347 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 56:
#line 533 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< Expression::Ptr > () = std::make_shared<DoubleLiteralExpression>(std::move(yystack_[0].value.as< std::string > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Float);
		}
#line 1356 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 57:
#line 538 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< Expression::Ptr > () = std::make_shared<StringLiteralExpression>(std::move(yystack_[0].value.as< std::string > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::String);
		}
#line 1365 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 58:
#line 543 "parser/yy/parser.y" // lalr1.cc:859
    {
			// Replace '#' for '$' to get string id
			auto stringId = yystack_[0].value.as< std::string > ();
			stringId[0] = '$';

			if (!driver.stringExists(stringId))
			{
				error(driver.getLocation(), "Reference to undefined string '" + yystack_[0].value.as< std::string > () + "'");
				YYABORT;
			}

			yylhs.value.as< Expression::Ptr > () = std::make_shared<StringCountExpression>(std::move(yystack_[0].value.as< std::string > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Int);
		}
#line 1384 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 59:
#line 558 "parser/yy/parser.y" // lalr1.cc:859
    {
			// Replace '@' for '$' to get string id
			auto stringId = yystack_[0].value.as< std::string > ();
			stringId[0] = '$';

			if (!driver.stringExists(stringId))
			{
				error(driver.getLocation(), "Reference to undefined string '" + yystack_[0].value.as< std::string > () + "'");
				YYABORT;
			}

			yylhs.value.as< Expression::Ptr > () = std::make_shared<StringOffsetExpression>(std::move(yystack_[0].value.as< std::string > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Int);
		}
#line 1403 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 60:
#line 573 "parser/yy/parser.y" // lalr1.cc:859
    {
			// Replace '@' for '$' to get string id
			auto stringId = yystack_[3].value.as< std::string > ();
			stringId[0] = '$';

			if (!driver.stringExists(stringId))
			{
				error(driver.getLocation(), "Reference to undefined string '" + yystack_[3].value.as< std::string > () + "'");
				YYABORT;
			}

			yylhs.value.as< Expression::Ptr > () = std::make_shared<StringOffsetExpression>(std::move(yystack_[3].value.as< std::string > ()), std::move(yystack_[1].value.as< Expression::Ptr > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Int);
		}
#line 1422 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 61:
#line 588 "parser/yy/parser.y" // lalr1.cc:859
    {
			// Replace '!' for '$' to get string id
			auto stringId = yystack_[0].value.as< std::string > ();
			stringId[0] = '$';

			if (!driver.stringExists(stringId))
			{
				error(driver.getLocation(), "Reference to undefined string '" + yystack_[0].value.as< std::string > () + "'");
				YYABORT;
			}

			yylhs.value.as< Expression::Ptr > () = std::make_shared<StringLengthExpression>(std::move(yystack_[0].value.as< std::string > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Int);
		}
#line 1441 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 62:
#line 603 "parser/yy/parser.y" // lalr1.cc:859
    {
			// Replace '!' for '$' to get string id
			auto stringId = yystack_[3].value.as< std::string > ();
			stringId[0] = '$';

			if (!driver.stringExists(stringId))
			{
				error(driver.getLocation(), "Reference to undefined string '" + yystack_[3].value.as< std::string > () + "'");
				YYABORT;
			}

			yylhs.value.as< Expression::Ptr > () = std::make_shared<StringLengthExpression>(std::move(yystack_[3].value.as< std::string > ()), std::move(yystack_[1].value.as< Expression::Ptr > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Int);
		}
#line 1460 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 63:
#line 618 "parser/yy/parser.y" // lalr1.cc:859
    {
			if (!yystack_[0].value.as< Expression::Ptr > ()->isInt() && !yystack_[0].value.as< Expression::Ptr > ()->isFloat())
			{
				error(driver.getLocation(), "unary minus expects integer or float type");
				YYABORT;
			}

			auto type = yystack_[0].value.as< Expression::Ptr > ()->getType();
			yylhs.value.as< Expression::Ptr > () = std::make_shared<UnaryMinusExpression>(std::move(yystack_[0].value.as< Expression::Ptr > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(type);
		}
#line 1476 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 64:
#line 630 "parser/yy/parser.y" // lalr1.cc:859
    {
			if (!yystack_[2].value.as< Expression::Ptr > ()->isInt() && !yystack_[2].value.as< Expression::Ptr > ()->isFloat())
			{
				error(driver.getLocation(), "operator '+' expects integer or float on the left-hand side");
				YYABORT;
			}

			if (!yystack_[0].value.as< Expression::Ptr > ()->isInt() && !yystack_[0].value.as< Expression::Ptr > ()->isFloat())
			{
				error(driver.getLocation(), "operator '+' expects integer or float on the right-hand side");
				YYABORT;
			}

			auto type = (yystack_[2].value.as< Expression::Ptr > ()->isInt() && yystack_[0].value.as< Expression::Ptr > ()->isInt()) ? Expression::Type::Int : Expression::Type::Float;
			yylhs.value.as< Expression::Ptr > () = std::make_shared<PlusExpression>(std::move(yystack_[2].value.as< Expression::Ptr > ()), std::move(yystack_[0].value.as< Expression::Ptr > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(type);
		}
#line 1498 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 65:
#line 648 "parser/yy/parser.y" // lalr1.cc:859
    {
			if (!yystack_[2].value.as< Expression::Ptr > ()->isInt() && !yystack_[2].value.as< Expression::Ptr > ()->isFloat())
			{
				error(driver.getLocation(), "operator '-' expects integer or float on the left-hand side");
				YYABORT;
			}

			if (!yystack_[0].value.as< Expression::Ptr > ()->isInt() && !yystack_[0].value.as< Expression::Ptr > ()->isFloat())
			{
				error(driver.getLocation(), "operator '-' expects integer or float on the right-hand side");
				YYABORT;
			}

			auto type = (yystack_[2].value.as< Expression::Ptr > ()->isInt() && yystack_[0].value.as< Expression::Ptr > ()->isInt()) ? Expression::Type::Int : Expression::Type::Float;
			yylhs.value.as< Expression::Ptr > () = std::make_shared<MinusExpression>(std::move(yystack_[2].value.as< Expression::Ptr > ()), std::move(yystack_[0].value.as< Expression::Ptr > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(type);
		}
#line 1520 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 66:
#line 666 "parser/yy/parser.y" // lalr1.cc:859
    {
			if (!yystack_[2].value.as< Expression::Ptr > ()->isInt() && !yystack_[2].value.as< Expression::Ptr > ()->isFloat())
			{
				error(driver.getLocation(), "operator '*' expects integer or float on the left-hand side");
				YYABORT;
			}

			if (!yystack_[0].value.as< Expression::Ptr > ()->isInt() && !yystack_[0].value.as< Expression::Ptr > ()->isFloat())
			{
				error(driver.getLocation(), "operator '*' expects integer or float on the right-hand side");
				YYABORT;
			}

			auto type = (yystack_[2].value.as< Expression::Ptr > ()->isInt() && yystack_[0].value.as< Expression::Ptr > ()->isInt()) ? Expression::Type::Int : Expression::Type::Float;
			yylhs.value.as< Expression::Ptr > () = std::make_shared<MultiplyExpression>(std::move(yystack_[2].value.as< Expression::Ptr > ()), std::move(yystack_[0].value.as< Expression::Ptr > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(type);
		}
#line 1542 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 67:
#line 684 "parser/yy/parser.y" // lalr1.cc:859
    {
			if (!yystack_[2].value.as< Expression::Ptr > ()->isInt() && !yystack_[2].value.as< Expression::Ptr > ()->isFloat())
			{
				error(driver.getLocation(), "operator '\\' expects integer or float on the left-hand side");
				YYABORT;
			}

			if (!yystack_[0].value.as< Expression::Ptr > ()->isInt() && !yystack_[0].value.as< Expression::Ptr > ()->isFloat())
			{
				error(driver.getLocation(), "operator '\\' expects integer or float on the right-hand side");
				YYABORT;
			}

			auto type = (yystack_[2].value.as< Expression::Ptr > ()->isInt() && yystack_[0].value.as< Expression::Ptr > ()->isInt()) ? Expression::Type::Int : Expression::Type::Float;
			yylhs.value.as< Expression::Ptr > () = std::make_shared<DivideExpression>(std::move(yystack_[2].value.as< Expression::Ptr > ()), std::move(yystack_[0].value.as< Expression::Ptr > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(type);
		}
#line 1564 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 68:
#line 702 "parser/yy/parser.y" // lalr1.cc:859
    {
			if (!yystack_[2].value.as< Expression::Ptr > ()->isInt())
			{
				error(driver.getLocation(), "operator '%' expects integer on the left-hand side");
				YYABORT;
			}

			if (!yystack_[0].value.as< Expression::Ptr > ()->isInt())
			{
				error(driver.getLocation(), "operator '%' expects integer on the right-hand side");
				YYABORT;
			}

			yylhs.value.as< Expression::Ptr > () = std::make_shared<ModuloExpression>(std::move(yystack_[2].value.as< Expression::Ptr > ()), std::move(yystack_[0].value.as< Expression::Ptr > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Int);
		}
#line 1585 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 69:
#line 719 "parser/yy/parser.y" // lalr1.cc:859
    {
			if (!yystack_[2].value.as< Expression::Ptr > ()->isInt())
			{
				error(driver.getLocation(), "operator '^' expects integer on the left-hand side");
				YYABORT;
			}

			if (!yystack_[0].value.as< Expression::Ptr > ()->isInt())
			{
				error(driver.getLocation(), "operator '^' expects integer on the right-hand side");
				YYABORT;
			}

			yylhs.value.as< Expression::Ptr > () = std::make_shared<BitwiseXorExpression>(std::move(yystack_[2].value.as< Expression::Ptr > ()), std::move(yystack_[0].value.as< Expression::Ptr > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Int);
		}
#line 1606 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 70:
#line 736 "parser/yy/parser.y" // lalr1.cc:859
    {
			if (!yystack_[2].value.as< Expression::Ptr > ()->isInt())
			{
				error(driver.getLocation(), "operator '&' expects integer on the left-hand side");
				YYABORT;
			}

			if (!yystack_[0].value.as< Expression::Ptr > ()->isInt())
			{
				error(driver.getLocation(), "operator '&' expects integer on the right-hand side");
				YYABORT;
			}

			yylhs.value.as< Expression::Ptr > () = std::make_shared<BitwiseAndExpression>(std::move(yystack_[2].value.as< Expression::Ptr > ()), std::move(yystack_[0].value.as< Expression::Ptr > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Int);
		}
#line 1627 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 71:
#line 753 "parser/yy/parser.y" // lalr1.cc:859
    {
			if (!yystack_[2].value.as< Expression::Ptr > ()->isInt())
			{
				error(driver.getLocation(), "operator '|' expects integer on the left-hand side");
				YYABORT;
			}

			if (!yystack_[0].value.as< Expression::Ptr > ()->isInt())
			{
				error(driver.getLocation(), "operator '|' expects integer on the right-hand side");
				YYABORT;
			}

			yylhs.value.as< Expression::Ptr > () = std::make_shared<BitwiseOrExpression>(std::move(yystack_[2].value.as< Expression::Ptr > ()), std::move(yystack_[0].value.as< Expression::Ptr > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Int);
		}
#line 1648 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 72:
#line 770 "parser/yy/parser.y" // lalr1.cc:859
    {
			if (!yystack_[0].value.as< Expression::Ptr > ()->isInt())
			{
				error(driver.getLocation(), "bitwise not expects integer");
				YYABORT;
			}

			yylhs.value.as< Expression::Ptr > () = std::make_shared<BitwiseNotExpression>(std::move(yystack_[0].value.as< Expression::Ptr > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Int);
		}
#line 1663 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 73:
#line 781 "parser/yy/parser.y" // lalr1.cc:859
    {
			if (!yystack_[2].value.as< Expression::Ptr > ()->isInt())
			{
				error(driver.getLocation(), "operator '<<' expects integer on the left-hand side");
				YYABORT;
			}

			if (!yystack_[0].value.as< Expression::Ptr > ()->isInt())
			{
				error(driver.getLocation(), "operator '<<' expects integer on the right-hand side");
				YYABORT;
			}

			yylhs.value.as< Expression::Ptr > () = std::make_shared<ShiftLeftExpression>(std::move(yystack_[2].value.as< Expression::Ptr > ()), std::move(yystack_[0].value.as< Expression::Ptr > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Int);
		}
#line 1684 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 74:
#line 798 "parser/yy/parser.y" // lalr1.cc:859
    {
			if (!yystack_[2].value.as< Expression::Ptr > ()->isInt())
			{
				error(driver.getLocation(), "operator '>>' expects integer on the left-hand side");
				YYABORT;
			}

			if (!yystack_[0].value.as< Expression::Ptr > ()->isInt())
			{
				error(driver.getLocation(), "operator '>>' expects integer on the right-hand side");
				YYABORT;
			}

			yylhs.value.as< Expression::Ptr > () = std::make_shared<ShiftRightExpression>(std::move(yystack_[2].value.as< Expression::Ptr > ()), std::move(yystack_[0].value.as< Expression::Ptr > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Int);
		}
#line 1705 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 75:
#line 815 "parser/yy/parser.y" // lalr1.cc:859
    {
			if (!yystack_[1].value.as< Expression::Ptr > ()->isInt())
			{
				error(driver.getLocation(), "operator '" + yystack_[3].value.as< std::string > () + "' expects integer");
				YYABORT;
			}

			yylhs.value.as< Expression::Ptr > () = std::make_shared<IntFunctionExpression>(std::move(yystack_[3].value.as< std::string > ()), std::move(yystack_[1].value.as< Expression::Ptr > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Int);
		}
#line 1720 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 76:
#line 826 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< Expression::Ptr > () = std::move(yystack_[0].value.as< Expression::Ptr > ());
		}
#line 1728 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 77:
#line 830 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< Expression::Ptr > () = std::make_shared<RegexpExpression>(std::move(yystack_[0].value.as< std::shared_ptr<yaramod::String> > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(Expression::Type::Regexp);
		}
#line 1737 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 78:
#line 838 "parser/yy/parser.y" // lalr1.cc:859
    {
			if (!yystack_[3].value.as< Expression::Ptr > ()->isInt())
			{
				error(driver.getLocation(), "operator '..' expects integer as lower bound of the interval");
				YYABORT;
			}

			if (!yystack_[1].value.as< Expression::Ptr > ()->isInt())
			{
				error(driver.getLocation(), "operator '..' expects integer as higher bound of the interval");
				YYABORT;
			}

			yylhs.value.as< Expression::Ptr > () = std::make_shared<RangeExpression>(std::move(yystack_[3].value.as< Expression::Ptr > ()), std::move(yystack_[1].value.as< Expression::Ptr > ()));
		}
#line 1757 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 79:
#line 856 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< Expression::Ptr > () = std::move(yystack_[0].value.as< Expression::Ptr > ()); }
#line 1763 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 80:
#line 857 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< Expression::Ptr > () = std::make_shared<AllExpression>(); }
#line 1769 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 81:
#line 858 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< Expression::Ptr > () = std::make_shared<AnyExpression>(); }
#line 1775 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 82:
#line 862 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< Expression::Ptr > () = std::make_shared<SetExpression>(std::move(yystack_[1].value.as< std::vector<Expression::Ptr> > ())); }
#line 1781 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 83:
#line 863 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< Expression::Ptr > () = std::move(yystack_[0].value.as< Expression::Ptr > ()); }
#line 1787 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 84:
#line 868 "parser/yy/parser.y" // lalr1.cc:859
    {
			if (!yystack_[0].value.as< Expression::Ptr > ()->isInt())
			{
				error(driver.getLocation(), "integer set expects integer type");
				YYABORT;
			}

			yylhs.value.as< std::vector<Expression::Ptr> > ().push_back(std::move(yystack_[0].value.as< Expression::Ptr > ()));
		}
#line 1801 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 85:
#line 878 "parser/yy/parser.y" // lalr1.cc:859
    {
			if (!yystack_[0].value.as< Expression::Ptr > ()->isInt())
			{
				error(driver.getLocation(), "integer set expects integer type");
				YYABORT;
			}

			yylhs.value.as< std::vector<Expression::Ptr> > () = std::move(yystack_[2].value.as< std::vector<Expression::Ptr> > ());
			yylhs.value.as< std::vector<Expression::Ptr> > ().push_back(std::move(yystack_[0].value.as< Expression::Ptr > ()));
		}
#line 1816 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 86:
#line 891 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< Expression::Ptr > () = std::make_shared<SetExpression>(std::move(yystack_[1].value.as< std::vector<Expression::Ptr> > ())); }
#line 1822 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 87:
#line 892 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< Expression::Ptr > () = std::make_shared<ThemExpression>(); }
#line 1828 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 88:
#line 897 "parser/yy/parser.y" // lalr1.cc:859
    {
			if (!driver.stringExists(yystack_[0].value.as< std::string > ()))
			{
				error(driver.getLocation(), "Reference to undefined string '" + yystack_[0].value.as< std::string > () + "'");
				YYABORT;
			}

			yylhs.value.as< std::vector<Expression::Ptr> > ().push_back(std::make_shared<StringExpression>(std::move(yystack_[0].value.as< std::string > ())));
		}
#line 1842 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 89:
#line 907 "parser/yy/parser.y" // lalr1.cc:859
    {
			if (!driver.stringExists(yystack_[0].value.as< std::string > ()))
			{
				error(driver.getLocation(), "No string matched with wildcard '" + yystack_[0].value.as< std::string > () + "'");
				YYABORT;
			}

			yylhs.value.as< std::vector<Expression::Ptr> > ().push_back(std::make_shared<StringWildcardExpression>(std::move(yystack_[0].value.as< std::string > ())));
		}
#line 1856 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 90:
#line 917 "parser/yy/parser.y" // lalr1.cc:859
    {
			if (!driver.stringExists(yystack_[0].value.as< std::string > ()))
			{
				error(driver.getLocation(), "Reference to undefined string '" + yystack_[0].value.as< std::string > () + "'");
				YYABORT;
			}

			yylhs.value.as< std::vector<Expression::Ptr> > () = std::move(yystack_[2].value.as< std::vector<Expression::Ptr> > ());
			yylhs.value.as< std::vector<Expression::Ptr> > ().push_back(std::make_shared<StringExpression>(std::move(yystack_[0].value.as< std::string > ())));
		}
#line 1871 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 91:
#line 928 "parser/yy/parser.y" // lalr1.cc:859
    {
			if (!driver.stringExists(yystack_[0].value.as< std::string > ()))
			{
				error(driver.getLocation(), "No string matched with wildcard '" + yystack_[0].value.as< std::string > () + "'");
				YYABORT;
			}

			yylhs.value.as< std::vector<Expression::Ptr> > () = std::move(yystack_[2].value.as< std::vector<Expression::Ptr> > ());
			yylhs.value.as< std::vector<Expression::Ptr> > ().push_back(std::make_shared<StringWildcardExpression>(std::move(yystack_[0].value.as< std::string > ())));
		}
#line 1886 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 92:
#line 942 "parser/yy/parser.y" // lalr1.cc:859
    {
			auto symbol = driver.findSymbol(yystack_[0].value.as< std::string > ());
			if (!symbol)
			{
				error(driver.getLocation(), "Unrecognized identifier '" + yystack_[0].value.as< std::string > () + "' referenced");
				YYABORT;
			}

			yylhs.value.as< Expression::Ptr > () = std::make_shared<IdExpression>(symbol);
			yylhs.value.as< Expression::Ptr > ()->setType(symbol->getDataType());
		}
#line 1902 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 93:
#line 954 "parser/yy/parser.y" // lalr1.cc:859
    {
			if (!yystack_[2].value.as< Expression::Ptr > ()->isObject())
			{
				error(driver.getLocation(), "Identifier '" + yystack_[2].value.as< Expression::Ptr > ()->getText() + "' is not an object");
				YYABORT;
			}

			auto parentSymbol = std::static_pointer_cast<const IdExpression>(yystack_[2].value.as< Expression::Ptr > ())->getSymbol();
			if (!parentSymbol->isStructure())
			{
				error(driver.getLocation(), "Identifier '" + parentSymbol->getName() + "' is not a structure");
				YYABORT;
			}

			auto structParentSymbol = std::static_pointer_cast<const StructureSymbol>(parentSymbol);
			auto attr = structParentSymbol->getAttribute(yystack_[0].value.as< std::string > ());
			if (!attr)
			{
				error(driver.getLocation(), "Unrecognized identifier '" + yystack_[0].value.as< std::string > () + "' referenced");
				YYABORT;
			}

			auto symbol = attr.value();
			yylhs.value.as< Expression::Ptr > () = std::make_shared<StructAccessExpression>(symbol, std::move(yystack_[2].value.as< Expression::Ptr > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(symbol->getDataType());
		}
#line 1933 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 94:
#line 981 "parser/yy/parser.y" // lalr1.cc:859
    {
			if (!yystack_[3].value.as< Expression::Ptr > ()->isObject())
			{
				error(driver.getLocation(), "Identifier '" + yystack_[3].value.as< Expression::Ptr > ()->getText() + "' is not an object");
				YYABORT;
			}

			auto parentSymbol = std::static_pointer_cast<const IdExpression>(yystack_[3].value.as< Expression::Ptr > ())->getSymbol();
			if (!parentSymbol->isArray() && !parentSymbol->isDictionary())
			{
				error(driver.getLocation(), "Identifier '" + parentSymbol->getName() + "' is not an array nor dictionary");
				YYABORT;
			}

			auto iterParentSymbol = std::static_pointer_cast<const IterableSymbol>(parentSymbol);
			yylhs.value.as< Expression::Ptr > () = std::make_shared<ArrayAccessExpression>(iterParentSymbol->getStructuredElementType(), std::move(yystack_[3].value.as< Expression::Ptr > ()), std::move(yystack_[1].value.as< Expression::Ptr > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(iterParentSymbol->getElementType());
		}
#line 1956 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 95:
#line 1000 "parser/yy/parser.y" // lalr1.cc:859
    {
			if (!yystack_[3].value.as< Expression::Ptr > ()->isObject())
			{
				error(driver.getLocation(), "Identifier '" + yystack_[3].value.as< Expression::Ptr > ()->getText() + "' is not an object");
				YYABORT;
			}

			auto parentSymbol = std::static_pointer_cast<const IdExpression>(yystack_[3].value.as< Expression::Ptr > ())->getSymbol();
			if (!parentSymbol->isFunction())
			{
				error(driver.getLocation(), "Identifier '" + parentSymbol->getName() + "' is not a function");
				YYABORT;
			}

			auto funcParentSymbol = std::static_pointer_cast<const FunctionSymbol>(parentSymbol);

			// Make copy of just argument types because symbols are not aware of expressions
			std::vector<Expression::Type> argTypes;
			std::for_each(yystack_[1].value.as< std::vector<Expression::Ptr> > ().begin(), yystack_[1].value.as< std::vector<Expression::Ptr> > ().end(),
				[&argTypes](const Expression::Ptr& expr)
				{
					argTypes.push_back(expr->getType());
				});

			if (!funcParentSymbol->overloadExists(argTypes))
			{
				error(driver.getLocation(), "No matching overload of function '" + funcParentSymbol->getName() + "' for these types of parameters");
				YYABORT;
			}

			yylhs.value.as< Expression::Ptr > () = std::make_shared<FunctionCallExpression>(std::move(yystack_[3].value.as< Expression::Ptr > ()), std::move(yystack_[1].value.as< std::vector<Expression::Ptr> > ()));
			yylhs.value.as< Expression::Ptr > ()->setType(funcParentSymbol->getReturnType());
		}
#line 1994 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 96:
#line 1037 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< std::vector<Expression::Ptr> > () = std::move(yystack_[2].value.as< std::vector<Expression::Ptr> > ());
			yylhs.value.as< std::vector<Expression::Ptr> > ().push_back(std::move(yystack_[0].value.as< Expression::Ptr > ()));
		}
#line 2003 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 97:
#line 1041 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::vector<Expression::Ptr> > ().push_back(std::move(yystack_[0].value.as< Expression::Ptr > ())); }
#line 2009 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 98:
#line 1042 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::vector<Expression::Ptr> > ().clear(); }
#line 2015 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 99:
#line 1046 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::uint32_t > () = yystack_[1].value.as< std::uint32_t > () | String::Modifiers::Ascii; }
#line 2021 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 100:
#line 1047 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::uint32_t > () = yystack_[1].value.as< std::uint32_t > () | String::Modifiers::Wide; }
#line 2027 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 101:
#line 1048 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::uint32_t > () = yystack_[1].value.as< std::uint32_t > () | String::Modifiers::Nocase; }
#line 2033 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 102:
#line 1049 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::uint32_t > () = yystack_[1].value.as< std::uint32_t > () | String::Modifiers::Fullword; }
#line 2039 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 103:
#line 1050 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::uint32_t > () = yystack_[1].value.as< std::uint32_t > () | String::Modifiers::Xor; }
#line 2045 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 104:
#line 1051 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::uint32_t > () = String::Modifiers::None; }
#line 2051 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 105:
#line 1055 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< yaramod::Literal > () = Literal(std::move(yystack_[0].value.as< std::string > ()), Literal::Type::String); }
#line 2057 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 106:
#line 1056 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< yaramod::Literal > () = Literal(std::move(yystack_[0].value.as< std::string > ()), Literal::Type::Int); }
#line 2063 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 107:
#line 1057 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< yaramod::Literal > () = Literal(yystack_[0].value.as< bool > ()); }
#line 2069 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 108:
#line 1061 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< bool > () = true; }
#line 2075 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 109:
#line 1062 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< bool > () = false; }
#line 2081 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 110:
#line 1066 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > () = std::move(yystack_[0].value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ()); }
#line 2087 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 111:
#line 1068 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > () = std::move(yystack_[2].value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ());
			yylhs.value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ().reserve(yylhs.value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ().size() + yystack_[1].value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ().size() + yystack_[0].value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ().size());
			std::move(yystack_[1].value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ().begin(), yystack_[1].value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ().end(), std::back_inserter(yylhs.value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ()));
			std::move(yystack_[0].value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ().begin(), yystack_[0].value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ().end(), std::back_inserter(yylhs.value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ()));
		}
#line 2098 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 112:
#line 1076 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > () = std::move(yystack_[0].value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ()); }
#line 2104 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 113:
#line 1077 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ().push_back(std::move(yystack_[0].value.as< std::shared_ptr<yaramod::HexStringUnit> > ())); }
#line 2110 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 114:
#line 1082 "parser/yy/parser.y" // lalr1.cc:859
    {
			auto first = std::make_shared<HexStringNibble>(yystack_[1].value.as< std::uint8_t > ());
			auto second = std::make_shared<HexStringNibble>(yystack_[0].value.as< std::uint8_t > ());
			yylhs.value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ().reserve(2);
			yylhs.value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ().push_back(std::move(first));
			yylhs.value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ().push_back(std::move(second));
		}
#line 2122 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 115:
#line 1090 "parser/yy/parser.y" // lalr1.cc:859
    {
			auto first = std::make_shared<HexStringNibble>(yystack_[1].value.as< std::uint8_t > ());
			auto second = std::make_shared<HexStringWildcard>();
			yylhs.value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ().reserve(2);
			yylhs.value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ().push_back(std::move(first));
			yylhs.value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ().push_back(std::move(second));
		}
#line 2134 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 116:
#line 1098 "parser/yy/parser.y" // lalr1.cc:859
    {
			auto first = std::make_shared<HexStringWildcard>();
			auto second = std::make_shared<HexStringNibble>(yystack_[0].value.as< std::uint8_t > ());
			yylhs.value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ().reserve(2);
			yylhs.value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ().push_back(std::move(first));
			yylhs.value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ().push_back(std::move(second));
		}
#line 2146 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 117:
#line 1106 "parser/yy/parser.y" // lalr1.cc:859
    {
			auto first = std::make_shared<HexStringWildcard>();
			auto second = std::make_shared<HexStringWildcard>();
			yylhs.value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ().reserve(2);
			yylhs.value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ().push_back(std::move(first));
			yylhs.value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ().push_back(std::move(second));
		}
#line 2158 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 118:
#line 1117 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > () = std::move(yystack_[1].value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ());
			std::move(yystack_[0].value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ().begin(), yystack_[0].value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ().end(), std::back_inserter(yylhs.value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ()));
		}
#line 2167 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 119:
#line 1122 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > () = std::move(yystack_[1].value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ());
			yylhs.value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ().push_back(std::move(yystack_[0].value.as< std::shared_ptr<yaramod::HexStringUnit> > ()));
		}
#line 2176 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 120:
#line 1127 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > () = std::move(yystack_[1].value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ());
			yylhs.value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ().push_back(std::move(yystack_[0].value.as< std::shared_ptr<yaramod::HexStringUnit> > ()));
		}
#line 2185 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 121:
#line 1131 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ().clear(); }
#line 2191 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 122:
#line 1135 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<yaramod::HexStringUnit> > () = std::make_shared<HexStringOr>(std::move(yystack_[1].value.as< std::vector<std::shared_ptr<yaramod::HexString>> > ())); }
#line 2197 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 123:
#line 1139 "parser/yy/parser.y" // lalr1.cc:859
    {
			auto hexStr = std::make_shared<HexString>(std::move(yystack_[0].value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ()));
			yylhs.value.as< std::vector<std::shared_ptr<yaramod::HexString>> > ().push_back(std::move(hexStr));
		}
#line 2206 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 124:
#line 1144 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< std::vector<std::shared_ptr<yaramod::HexString>> > () = std::move(yystack_[2].value.as< std::vector<std::shared_ptr<yaramod::HexString>> > ());
			auto hexStr = std::make_shared<HexString>(std::move(yystack_[0].value.as< std::vector<std::shared_ptr<yaramod::HexStringUnit>> > ()));
			yylhs.value.as< std::vector<std::shared_ptr<yaramod::HexString>> > ().push_back(std::move(hexStr));
		}
#line 2216 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 125:
#line 1152 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<yaramod::HexStringUnit> > () = std::make_shared<HexStringJump>(yystack_[1].value.as< std::uint64_t > (), yystack_[1].value.as< std::uint64_t > ()); }
#line 2222 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 126:
#line 1153 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<yaramod::HexStringUnit> > () = std::make_shared<HexStringJump>(yystack_[3].value.as< std::uint64_t > (), yystack_[1].value.as< std::uint64_t > ()); }
#line 2228 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 127:
#line 1154 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<yaramod::HexStringUnit> > () = std::make_shared<HexStringJump>(yystack_[2].value.as< std::uint64_t > ()); }
#line 2234 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 128:
#line 1155 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<yaramod::HexStringUnit> > () = std::make_shared<HexStringJump>(); }
#line 2240 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 129:
#line 1160 "parser/yy/parser.y" // lalr1.cc:859
    {
			driver.getLexer().switchToRegexpLexer();
		}
#line 2248 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 130:
#line 1164 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< std::shared_ptr<yaramod::String> > () = std::move(yystack_[1].value.as< std::shared_ptr<yaramod::String> > ());
			std::static_pointer_cast<Regexp>(yylhs.value.as< std::shared_ptr<yaramod::String> > ())->setSuffixModifiers(yystack_[0].value.as< std::string > ());
			driver.getLexer().switchToYaraLexer();
		}
#line 2258 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 131:
#line 1171 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<yaramod::String> > () = std::make_shared<Regexp>(std::move(yystack_[0].value.as< std::shared_ptr<yaramod::RegexpUnit> > ())); }
#line 2264 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 132:
#line 1175 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<yaramod::RegexpUnit> > () = std::make_shared<RegexpConcat>(std::move(yystack_[0].value.as< std::vector<std::shared_ptr<yaramod::RegexpUnit>> > ())); }
#line 2270 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 133:
#line 1177 "parser/yy/parser.y" // lalr1.cc:859
    {
			auto concat = std::make_shared<RegexpConcat>(std::move(yystack_[0].value.as< std::vector<std::shared_ptr<yaramod::RegexpUnit>> > ()));
			yylhs.value.as< std::shared_ptr<yaramod::RegexpUnit> > () = std::make_shared<RegexpOr>(std::move(yystack_[2].value.as< std::shared_ptr<yaramod::RegexpUnit> > ()), std::move(concat));
		}
#line 2279 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 134:
#line 1184 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::vector<std::shared_ptr<yaramod::RegexpUnit>> > ().push_back(std::move(yystack_[0].value.as< std::shared_ptr<yaramod::RegexpUnit> > ())); }
#line 2285 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 135:
#line 1186 "parser/yy/parser.y" // lalr1.cc:859
    {
			yylhs.value.as< std::vector<std::shared_ptr<yaramod::RegexpUnit>> > () = std::move(yystack_[1].value.as< std::vector<std::shared_ptr<yaramod::RegexpUnit>> > ());
			yylhs.value.as< std::vector<std::shared_ptr<yaramod::RegexpUnit>> > ().push_back(std::move(yystack_[0].value.as< std::shared_ptr<yaramod::RegexpUnit> > ()));
		}
#line 2294 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 136:
#line 1193 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<yaramod::RegexpUnit> > () = std::make_shared<RegexpIteration>(std::move(yystack_[2].value.as< std::shared_ptr<yaramod::RegexpUnit> > ()), yystack_[0].value.as< bool > ()); }
#line 2300 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 137:
#line 1194 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<yaramod::RegexpUnit> > () = std::make_shared<RegexpPositiveIteration>(std::move(yystack_[2].value.as< std::shared_ptr<yaramod::RegexpUnit> > ()), yystack_[0].value.as< bool > ()); }
#line 2306 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 138:
#line 1195 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<yaramod::RegexpUnit> > () = std::make_shared<RegexpOptional>(std::move(yystack_[2].value.as< std::shared_ptr<yaramod::RegexpUnit> > ()), yystack_[0].value.as< bool > ()); }
#line 2312 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 139:
#line 1197 "parser/yy/parser.y" // lalr1.cc:859
    {
			if (!yystack_[1].value.as< yaramod::RegexpRangePair > ().first && !yystack_[1].value.as< yaramod::RegexpRangePair > ().second)
			{
				error(driver.getLocation(), "Range in regular expression does not have defined lower bound nor higher bound");
				YYABORT;
			}

			if (yystack_[1].value.as< yaramod::RegexpRangePair > ().first && yystack_[1].value.as< yaramod::RegexpRangePair > ().second && yystack_[1].value.as< yaramod::RegexpRangePair > ().first.value() > yystack_[1].value.as< yaramod::RegexpRangePair > ().second.value())
			{
				error(driver.getLocation(), "Range in regular expression has greater lower bound than higher bound");
				YYABORT;
			}

			yylhs.value.as< std::shared_ptr<yaramod::RegexpUnit> > () = std::make_shared<RegexpRange>(std::move(yystack_[2].value.as< std::shared_ptr<yaramod::RegexpUnit> > ()), std::move(yystack_[1].value.as< yaramod::RegexpRangePair > ()), yystack_[0].value.as< bool > ());
		}
#line 2332 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 140:
#line 1212 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<yaramod::RegexpUnit> > () = std::move(yystack_[0].value.as< std::shared_ptr<yaramod::RegexpUnit> > ()); }
#line 2338 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 141:
#line 1213 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<yaramod::RegexpUnit> > () = std::make_shared<RegexpWordBoundary>(); }
#line 2344 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 142:
#line 1214 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<yaramod::RegexpUnit> > () = std::make_shared<RegexpNonWordBoundary>(); }
#line 2350 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 143:
#line 1215 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<yaramod::RegexpUnit> > () = std::make_shared<RegexpStartOfLine>(); }
#line 2356 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 144:
#line 1216 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<yaramod::RegexpUnit> > () = std::make_shared<RegexpEndOfLine>(); }
#line 2362 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 145:
#line 1220 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< bool > () = true; }
#line 2368 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 146:
#line 1221 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< bool > () = false; }
#line 2374 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 147:
#line 1225 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<yaramod::RegexpUnit> > () = std::make_shared<RegexpGroup>(std::move(yystack_[1].value.as< std::shared_ptr<yaramod::RegexpUnit> > ())); }
#line 2380 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 148:
#line 1226 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<yaramod::RegexpUnit> > () = std::make_shared<RegexpAnyChar>(); }
#line 2386 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 149:
#line 1227 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<yaramod::RegexpUnit> > () = std::make_shared<RegexpText>(std::move(yystack_[0].value.as< std::string > ())); }
#line 2392 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 150:
#line 1228 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<yaramod::RegexpUnit> > () = std::make_shared<RegexpWordChar>(); }
#line 2398 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 151:
#line 1229 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<yaramod::RegexpUnit> > () = std::make_shared<RegexpNonWordChar>(); }
#line 2404 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 152:
#line 1230 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<yaramod::RegexpUnit> > () = std::make_shared<RegexpSpace>(); }
#line 2410 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 153:
#line 1231 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<yaramod::RegexpUnit> > () = std::make_shared<RegexpNonSpace>(); }
#line 2416 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 154:
#line 1232 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<yaramod::RegexpUnit> > () = std::make_shared<RegexpDigit>(); }
#line 2422 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 155:
#line 1233 "parser/yy/parser.y" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<yaramod::RegexpUnit> > () = std::make_shared<RegexpNonDigit>(); }
#line 2428 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;

  case 156:
#line 1235 "parser/yy/parser.y" // lalr1.cc:859
    {
			// It is negative class
			if (yystack_[0].value.as< std::string > ()[0] == '^')
				yylhs.value.as< std::shared_ptr<yaramod::RegexpUnit> > () = std::make_shared<RegexpClass>(yystack_[0].value.as< std::string > ().substr(1, yystack_[0].value.as< std::string > ().length() - 1), true);
			else
				yylhs.value.as< std::shared_ptr<yaramod::RegexpUnit> > () = std::make_shared<RegexpClass>(std::move(yystack_[0].value.as< std::string > ()), false);
		}
#line 2440 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
    break;


#line 2444 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:859
            default:
              break;
            }
        }
      catch (const syntax_error& yyexc)
        {
          error (yyexc);
          YYERROR;
        }
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, yylhs);
    }
    goto yynewstate;

  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        error (yyla.location, yysyntax_error_ (yystack_[0].state, yyla));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;
    yyerror_range[1].location = yystack_[yylen - 1].location;
    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[yystack_[0].state];
          if (!yy_pact_value_is_default_ (yyn))
            {
              yyn += yyterror_;
              if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
                {
                  yyn = yytable_[yyn];
                  if (0 < yyn)
                    break;
                }
            }

          // Pop the current state because it cannot handle the error token.
          if (yystack_.size () == 1)
            YYABORT;

          yyerror_range[1].location = yystack_[0].location;
          yy_destroy_ ("Error: popping", yystack_[0]);
          yypop_ ();
          YY_STACK_PRINT ();
        }

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = yyn;
      yypush_ ("Shifting", error_token);
    }
    goto yynewstate;

    // Accept.
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    // Abort.
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack"
                 << std::endl;
        // Do not try to display the values of the reclaimed symbols,
        // as their printer might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
  }

  void
   Parser ::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what());
  }

  // Generate an error message.
  std::string
   Parser ::yysyntax_error_ (state_type yystate, const symbol_type& yyla) const
  {
    // Number of reported tokens (one for the "unexpected", one per
    // "expected").
    size_t yycount = 0;
    // Its maximum.
    enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
    // Arguments of yyformat.
    char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];

    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state
         merging (from LALR or IELR) and default reductions corrupt the
         expected token list.  However, the list is correct for
         canonical LR with one exception: it will still contain any
         token that will not be accepted due to an error action in a
         later state.
    */
    if (!yyla.empty ())
      {
        int yytoken = yyla.type_get ();
        yyarg[yycount++] = yytname_[yytoken];
        int yyn = yypact_[yystate];
        if (!yy_pact_value_is_default_ (yyn))
          {
            /* Start YYX at -YYN if negative to avoid negative indexes in
               YYCHECK.  In other words, skip the first -YYN actions for
               this state because they are default actions.  */
            int yyxbegin = yyn < 0 ? -yyn : 0;
            // Stay within bounds of both yycheck and yytname.
            int yychecklim = yylast_ - yyn + 1;
            int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
            for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
              if (yycheck_[yyx + yyn] == yyx && yyx != yyterror_
                  && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
                {
                  if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                    {
                      yycount = 1;
                      break;
                    }
                  else
                    yyarg[yycount++] = yytname_[yyx];
                }
          }
      }

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
        YYCASE_(0, YY_("syntax error"));
        YYCASE_(1, YY_("syntax error, unexpected %s"));
        YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    size_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += yytnamerr_ (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const short int  Parser ::yypact_ninf_ = -157;

  const signed char  Parser ::yytable_ninf_ = -114;

  const short int
   Parser ::yypact_[] =
  {
    -157,     4,  -157,  -157,  -157,  -157,   -22,  -157,  -157,    51,
    -157,  -157,    48,  -157,   116,    83,   143,  -157,   120,   151,
    -157,   167,   174,  -157,   184,   176,   147,  -157,   189,   198,
     203,   169,   138,  -157,   107,   205,   204,   204,   138,  -157,
    -157,   138,  -157,  -157,   187,  -157,  -157,  -157,  -157,  -157,
    -157,    47,   166,   168,  -157,  -157,   214,   117,   267,   191,
       0,  -157,  -157,  -157,  -157,  -157,  -157,   -19,   204,  -157,
    -157,   119,   121,  -157,   352,    18,    23,   204,   218,   204,
     204,   204,   138,   138,   204,   204,   204,   204,   204,   204,
     204,   204,   204,   204,   204,   204,   204,   204,   204,   204,
     204,   185,   123,   177,   138,   204,  -157,  -157,  -157,  -157,
     313,  -157,  -157,   123,  -157,    23,  -157,  -157,  -157,  -157,
    -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,   192,
     173,    23,  -157,    77,   352,   204,  -157,    -3,    76,   326,
    -157,   212,   352,   352,   352,   352,   352,   352,   303,   303,
     141,   141,  -157,  -157,  -157,   362,   278,   295,   352,  -157,
     -34,  -157,  -157,  -157,   117,     1,   105,    -1,   347,   347,
    -157,   206,     2,  -157,    23,  -157,   179,   179,   179,   179,
     285,  -157,  -157,  -157,  -157,  -157,     7,  -157,   138,  -157,
    -157,    79,   100,   235,   240,  -157,  -157,  -157,  -157,  -157,
    -157,  -157,   239,   299,  -157,    23,  -157,  -157,  -157,  -157,
    -157,   204,  -157,   129,   117,    10,     3,  -157,  -157,  -157,
    -157,  -157,    10,   312,   204,  -157,   308,   339,  -157,  -157,
     108,  -157,  -157,  -157,  -157,  -157,  -157,   322,   323,   138,
     285,    57,   338,  -157,   291,   152,    10,   170,  -157,   204,
     138,  -157,  -157,    27,  -157,   352,   183,  -157,   310,  -157,
    -157
  };

  const unsigned char
   Parser ::yydefact_[] =
  {
       5,    12,     1,     4,    11,    10,     0,     3,     2,     0,
       6,     7,     0,     8,    14,     0,     0,    16,    13,    18,
      15,     0,    22,    20,     0,     0,    17,    24,     0,     0,
       0,    21,     0,     9,     0,     0,     0,     0,     0,   108,
     109,     0,    80,    81,     0,    54,    53,   129,    57,    55,
      56,    31,    61,    59,    58,    92,     0,    29,    50,     0,
      76,    30,    77,   105,   106,    19,   107,     0,     0,    63,
      72,     0,    50,    39,    79,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    98,     0,    26,   104,    23,   104,
       0,    51,    52,     0,    34,     0,   143,   144,   148,   150,
     151,   152,   153,   154,   155,   141,   142,   149,   156,     0,
     131,   132,   134,   140,    32,     0,    33,     0,     0,     0,
      40,    41,    42,    43,    44,    45,    46,    47,    73,    74,
      65,    64,    66,    67,    68,    69,    70,    71,    48,    49,
       0,    87,    38,    93,    97,     0,     0,     0,    25,    28,
      36,     0,     0,   130,     0,   135,   145,   145,   145,   145,
       0,    62,    60,    75,    88,    89,     0,    95,     0,    94,
     121,     0,     0,     0,   121,   112,   113,    99,   101,   100,
     102,   103,     0,     0,   147,   133,   146,   136,   137,   138,
     139,     0,    86,     0,    96,   123,     0,   117,   116,   115,
     114,    27,     0,     0,     0,    83,     0,     0,    90,    91,
       0,   118,   119,   120,   122,   121,   111,   118,   119,     0,
      84,     0,     0,    78,     0,     0,   124,     0,    82,     0,
       0,   128,   125,     0,    37,    85,     0,   127,     0,    35,
     126
  };

  const short int
   Parser ::yypgoto_[] =
  {
    -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,
    -157,  -157,  -157,  -157,  -157,  -157,   -17,  -157,  -157,   -36,
     186,   344,  -157,  -157,   277,  -157,  -157,  -157,   282,  -157,
     358,  -157,   171,  -148,  -156,  -147,  -157,  -157,   -25,  -157,
    -157,   279,   221,  -128,    42,  -157
  };

  const short int
   Parser ::yydefgoto_[] =
  {
      -1,     1,     7,     8,    12,    14,     9,    16,    18,    22,
      26,    25,    31,   108,   167,    29,    57,   171,   202,    58,
     136,    59,   226,   241,   162,   186,    60,   165,   168,    65,
      61,   193,   194,   231,   215,   232,   216,   233,    62,    76,
     129,   130,   131,   132,   207,   133
  };

  const short int
   Parser ::yytable_[] =
  {
      69,    70,    72,   175,     2,   103,   106,     3,    74,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   195,
     196,    71,   190,   104,    73,   187,   204,   234,   184,   185,
     188,   212,   110,   190,     4,     5,   213,    10,   222,    47,
     107,   134,   109,   137,   138,   139,   115,     6,   142,   143,
     144,   145,   146,   147,   148,   149,   150,   151,   152,   153,
     154,   155,   156,   157,   158,   140,   141,   113,   181,   166,
     105,   191,   235,   192,   237,   238,   159,   175,   174,   246,
     230,   248,   191,    11,   192,   114,   249,   164,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   257,   180,
      77,    78,   258,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,    13,   128,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   111,    15,   112,   160,   182,    39,    40,
      17,   217,    36,   218,   176,   177,   178,    94,    95,    96,
      37,    38,    82,    83,    82,    83,    63,    64,    19,   179,
     -79,   214,   219,   161,   220,   227,   189,   100,   101,    39,
      40,   244,    41,   245,    21,    42,    43,    20,   240,    44,
      45,   228,   229,    46,   254,    23,    47,    48,    49,    50,
      51,    36,    52,    53,    54,    55,    56,   259,    24,    37,
      68,    28,    27,   255,    30,    82,    83,    32,    36,   208,
     209,   210,   247,   252,    33,   253,    37,    68,    82,    83,
      34,    35,    67,   256,    42,    43,    79,    81,    80,    45,
     102,   135,    46,    47,   163,    47,    48,    49,    50,   174,
     173,    52,    53,    54,    55,    56,    45,    82,   206,    46,
     203,   221,    47,    48,    49,    50,  -110,   223,    52,    53,
      54,    55,    56,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   211,
      90,    91,    92,    93,    94,    95,    96,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,    90,    91,    92,
      93,    94,    95,    96,    97,    98,   -79,    92,    93,    94,
      95,    96,   224,   100,   101,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   239,   242,   112,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,  -112,  -113,
     183,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   250,   251,   243,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,    90,    91,    92,    93,    94,    95,
      96,   260,    98,   197,   198,   199,   200,   201,    75,   225,
     170,   169,    66,   236,   172,   205
  };

  const unsigned char
   Parser ::yycheck_[] =
  {
      36,    37,    38,   131,     0,     5,    25,     3,    44,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,   167,
     167,    38,    23,    23,    41,    24,    24,    24,    62,    63,
      29,    24,    68,    23,    30,    31,    29,    59,   194,    58,
      59,    77,    67,    79,    80,    81,    23,    43,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,    82,    83,    49,    71,   105,
      70,    72,    69,    74,   222,   222,   101,   205,    76,   235,
      70,    24,    72,    32,    74,    67,    29,   104,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    71,   135,
      53,    54,    75,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    67,    93,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    24,    28,    24,    23,    71,    41,    42,
      67,    72,    14,    74,    77,    78,    79,    16,    17,    18,
      22,    23,    45,    46,    45,    46,    59,    60,    25,    92,
      49,   188,    72,    50,    74,   211,    71,    56,    57,    41,
      42,    73,    44,    75,    33,    47,    48,    67,   224,    51,
      52,    62,    63,    55,    24,    28,    58,    59,    60,    61,
      62,    14,    64,    65,    66,    67,    68,    24,    34,    22,
      23,    35,    28,   249,    67,    45,    46,    28,    14,   177,
     178,   179,   239,    71,    26,    73,    22,    23,    45,    46,
      27,    62,    27,   250,    47,    48,    70,    23,    70,    52,
      49,    23,    55,    58,    67,    58,    59,    60,    61,    76,
      58,    64,    65,    66,    67,    68,    52,    45,    79,    55,
      54,    26,    58,    59,    60,    61,    26,    28,    64,    65,
      66,    67,    68,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,     4,
      12,    13,    14,    15,    16,    17,    18,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    49,    14,    15,    16,
      17,    18,    23,    56,    57,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    23,    28,    24,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    26,    26,
      24,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    23,    71,    24,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    12,    13,    14,    15,    16,    17,
      18,    71,    20,    36,    37,    38,    39,    40,    44,   203,
     113,   109,    34,   222,   115,   174
  };

  const unsigned char
   Parser ::yystos_[] =
  {
       0,    96,     0,     3,    30,    31,    43,    97,    98,   101,
      59,    32,    99,    67,   100,    28,   102,    67,   103,    25,
      67,    33,   104,    28,    34,   106,   105,    28,    35,   110,
      67,   107,    28,    26,    27,    62,    14,    22,    23,    41,
      42,    44,    47,    48,    51,    52,    55,    58,    59,    60,
      61,    62,    64,    65,    66,    67,    68,   111,   114,   116,
     121,   125,   133,    59,    60,   124,   125,    27,    23,   114,
     114,   111,   114,   111,   114,   116,   134,    53,    54,    70,
      70,    23,    45,    46,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      56,    57,    49,     5,    23,    70,    25,    59,   108,   133,
     114,    24,    24,    49,    67,    23,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    93,   135,
     136,   137,   138,   140,   114,    23,   115,   114,   114,   114,
     111,   111,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   133,
      23,    50,   119,    67,   111,   122,   114,   109,   123,   123,
     119,   112,   136,    58,    76,   138,    77,    78,    79,    92,
     114,    71,    71,    24,    62,    63,   120,    24,    29,    71,
      23,    72,    74,   126,   127,   128,   130,    36,    37,    38,
      39,    40,   113,    54,    24,   137,    79,   139,   139,   139,
     139,     4,    24,    29,   111,   129,   131,    72,    74,    72,
      74,    26,   129,    28,    23,   115,   117,   114,    62,    63,
      70,   128,   130,   132,    24,    69,   127,   128,   130,    23,
     114,   118,    28,    24,    73,    75,   129,   111,    24,    29,
      23,    71,    71,    73,    24,   114,   111,    71,    75,    24,
      71
  };

  const unsigned char
   Parser ::yyr1_[] =
  {
       0,    95,    96,    96,    96,    96,    97,    99,   100,    98,
     101,   101,   101,   102,   102,   103,   103,   104,   104,   105,
     105,   106,   106,   107,   107,   108,   109,   108,   108,   110,
     111,   111,   111,   111,   112,   111,   113,   111,   111,   111,
     111,   111,   111,   111,   111,   111,   111,   111,   111,   111,
     111,   111,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   115,   116,
     116,   116,   117,   117,   118,   118,   119,   119,   120,   120,
     120,   120,   121,   121,   121,   121,   122,   122,   122,   123,
     123,   123,   123,   123,   123,   124,   124,   124,   125,   125,
     126,   126,   127,   127,   128,   128,   128,   128,   129,   129,
     129,   129,   130,   131,   131,   132,   132,   132,   132,   134,
     133,   135,   136,   136,   137,   137,   138,   138,   138,   138,
     138,   138,   138,   138,   138,   139,   139,   140,   140,   140,
     140,   140,   140,   140,   140,   140,   140
  };

  const unsigned char
   Parser ::yyr2_[] =
  {
       0,     2,     2,     2,     2,     0,     2,     0,     0,    11,
       1,     1,     0,     2,     0,     2,     1,     3,     0,     4,
       0,     3,     0,     4,     0,     2,     0,     4,     2,     3,
       1,     1,     3,     3,     0,    10,     0,     9,     3,     2,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       1,     3,     3,     1,     1,     1,     1,     1,     1,     1,
       4,     1,     4,     2,     3,     3,     3,     3,     3,     3,
       3,     3,     2,     3,     3,     4,     1,     1,     5,     1,
       1,     1,     3,     1,     1,     3,     3,     1,     1,     1,
       3,     3,     1,     3,     4,     4,     3,     1,     0,     2,
       2,     2,     2,     2,     0,     1,     1,     1,     1,     1,
       1,     3,     1,     1,     2,     2,     2,     2,     2,     2,
       2,     0,     3,     1,     3,     3,     5,     4,     3,     0,
       4,     1,     1,     3,     1,     2,     3,     3,     3,     3,
       1,     1,     1,     1,     1,     0,     1,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1
  };



  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const  Parser ::yytname_[] =
  {
  "$end", "error", "$undefined", "\"end of file\"", "\"integer range\"",
  "\".\"", "\"<\"", "\">\"", "\"<=\"", "\">=\"", "\"==\"", "\"!=\"",
  "\"<<\"", "\">>\"", "\"-\"", "\"+\"", "\"*\"", "\"\\\\\"", "\"%\"",
  "\"^\"", "\"&\"", "\"|\"", "\"~\"", "\"(\"", "\")\"", "\"{\"", "\"}\"",
  "\"=\"", "\":\"", "\",\"", "\"private\"", "\"global\"", "\"rule\"",
  "\"meta\"", "\"strings\"", "\"condition\"", "\"ascii\"", "\"nocase\"",
  "\"wide\"", "\"fullword\"", "\"xor\"", "\"true\"", "\"false\"",
  "\"import\"", "\"not\"", "\"and\"", "\"or\"", "\"all\"", "\"any\"",
  "\"of\"", "\"them\"", "\"for\"", "\"entrypoint\"", "\"at\"", "\"in\"",
  "\"filesize\"", "\"contains\"", "\"matches\"", "\"/\"",
  "\"string literal\"", "\"integer\"", "\"float\"",
  "\"string identifier\"", "\"string wildcard\"", "\"string length\"",
  "\"string offset\"", "\"string count\"", "\"identifier\"",
  "\"fixed-width integer function\"", "\"hex string |\"",
  "\"hex string [\"", "\"hex string ]\"", "\"hex string ?\"",
  "\"hex string -\"", "\"hex string nibble\"", "\"hex string integer\"",
  "\"regexp |\"", "\"regexp *\"", "\"regexp +\"", "\"regexp ?\"",
  "\"regexp ^\"", "\"regexp $\"", "\"regexp .\"", "\"regexp \\\\w\"",
  "\"regexp \\\\W\"", "\"regexp \\\\s\"", "\"regexp \\\\S\"",
  "\"regexp \\\\d\"", "\"regexp \\\\D\"", "\"regexp \\\\b\"",
  "\"regexp \\\\B\"", "\"regexp character\"", "\"regexp range\"",
  "\"regexp class\"", "UNARY_MINUS", "$accept", "rules", "import", "rule",
  "$@1", "$@2", "rule_mod", "tags", "tag_list", "metas", "metas_body",
  "strings", "strings_body", "string", "$@3", "condition", "expression",
  "$@4", "$@5", "primary_expression", "range", "for_expression",
  "integer_set", "integer_enumeration", "string_set", "string_enumeration",
  "identifier", "arguments", "string_mods", "literal", "boolean",
  "hex_string", "hex_string_edge", "hex_byte", "hex_string_body", "hex_or",
  "hex_or_body", "hex_jump", "regexp", "$@6", "regexp_body", "regexp_or",
  "regexp_concat", "regexp_repeat", "regexp_greedy", "regexp_single", YY_NULLPTR
  };

#if YYDEBUG
  const unsigned short int
   Parser ::yyrline_[] =
  {
       0,   217,   217,   218,   219,   220,   224,   236,   240,   235,
     254,   255,   256,   260,   261,   265,   270,   274,   275,   279,
     284,   288,   289,   297,   308,   316,   322,   321,   330,   338,
     342,   347,   358,   375,   387,   386,   403,   402,   419,   424,
     429,   434,   439,   444,   449,   454,   459,   464,   469,   486,
     498,   502,   511,   517,   522,   527,   532,   537,   542,   557,
     572,   587,   602,   617,   629,   647,   665,   683,   701,   718,
     735,   752,   769,   780,   797,   814,   825,   829,   837,   856,
     857,   858,   862,   863,   867,   877,   891,   892,   896,   906,
     916,   927,   941,   953,   980,   999,  1036,  1041,  1042,  1046,
    1047,  1048,  1049,  1050,  1051,  1055,  1056,  1057,  1061,  1062,
    1066,  1067,  1076,  1077,  1081,  1089,  1097,  1105,  1116,  1121,
    1126,  1131,  1135,  1138,  1143,  1152,  1153,  1154,  1155,  1160,
    1159,  1171,  1175,  1176,  1184,  1185,  1193,  1194,  1195,  1196,
    1212,  1213,  1214,  1215,  1216,  1220,  1221,  1225,  1226,  1227,
    1228,  1229,  1230,  1231,  1232,  1233,  1234
  };

  // Print the state stack on the debug stream.
  void
   Parser ::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << i->state;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
   Parser ::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):" << std::endl;
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG


#line 61 "parser/yy/parser.y" // lalr1.cc:1167
} } //  yaramod::yy 
#line 3044 "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-build/src/yaramod/yy/yy_parser.cpp" // lalr1.cc:1167
#line 1244 "parser/yy/parser.y" // lalr1.cc:1168


// Bison expects implementation of error method by us
void yy::Parser::error(const yy::location& loc, const std::string& message)
{
	std::ostringstream os;
	os << "Error at " << loc << ": " << message;
	throw ParserError(os.str());
}
