/*! \file expression.hpp
Defines the Expression type and assiciated functions.
 */
#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <string>
#include <vector>
#include <map>

#include "token.hpp"
#include "atom.hpp"

// forward declare Environment
class Environment;

/*! \class Expression
\brief An expression is a tree of Atoms.

An expression is an atom called the head followed by a (possibly empty) 
list of expressions called the tail.
 */
class Expression {
public:

  typedef std::vector<Expression>::const_iterator ConstIteratorType;

  /// Default construct and Expression, whose type in NoneType
  Expression();

  /*! Construct an Expression with given Atom as head an empty tail
    \param atom the atom to make the head
  */
  Expression(const Atom & a);

  /// deep-copy construct an expression (recursive)
  Expression(const Expression & a);

  /// deep-copy assign an expression  (recursive)
  Expression & operator=(const Expression & a);

  /// return a reference to the head Atom
  Atom & head();

  /// return a const-reference to the head Atom
  const Atom & head() const;

  /// append Atom to tail of the expression
  void append(const Atom & a);

  /// append Expression to the tail of the expression
  void appendExpression(const Expression & a);

  /// return a pointer to the last expression in the tail, or nullptr
  Expression * tail();

 // bool isEqual(Expression & exp, std::vector<Expression> args);

  /// return a const-iterator to the beginning of tail
  ConstIteratorType tailConstBegin() const noexcept;

  /// return a const-iterator to the tail end
  ConstIteratorType tailConstEnd() const noexcept;

  /// convienience member to determine if head atom is a number
  bool isHeadNumber() const noexcept;

  /// convienience member to determine if head atom is a symbol
  bool isHeadSymbol() const noexcept;

  /// comvenience member to determine if head atom is a complex
  bool isHeadComplex() const noexcept;

  /// Evaluate expression using a post-order traversal (recursive)
  Expression eval(Environment & env);

  /// equality comparison for two expressions (recursive)
  bool operator==(const Expression & exp) const noexcept;
  
  Expression getProperty(std::string str);
  

private:

  // the head of the expression
  Atom m_head;

  // the tail list is expressed as a vector for access efficiency
  // and cache coherence, at the cost of wasted memory.
  std::vector<Expression> m_tail;

  // convenience typedef
  typedef std::vector<Expression>::iterator IteratorType;
  
  // internal helper methods
  Expression handle_lookup(const Atom & head, const Environment & env);
  Expression handle_define(Environment & env);
  Expression doApply(Environment & env);
  Expression doMap(Environment & env, Environment & lambdaEnv);
  Expression doLambda(Environment &lambdaEnv, Environment &env, Expression check);
  Expression doSetProperty(Environment & env);
  Expression doGetProperty(Environment & env);
  Expression handle_begin(Environment & env);
  Expression handle_lambda(Environment & env);
  Expression do_discrete_plot(Environment & env);
  Expression do_continuous_plot(Environment & env);
  Expression create_data_pairs(Expression tail_1, Environment & env);
  Expression make_plot_bound(Environment &env, Expression origin_head, Expression &output);
 // Expression make_plot_labels(Environment &env, Expression origin_head);
  void get_min_max(Expression tail_0, Expression tail_1);
  Expression get_labels(Expression tail_1, Expression & output, int plot);
  bool map_contains(std::string key);
  Expression make_plot_coords(Environment &env, Expression origin_head, bool y_overlap, bool x_overlap, Expression &output);
  Expression plot_data(Environment &env, Expression tail_0, Expression &output, bool x_overlap, int plot);

  //std::map<std::string, std::string> propertyMap;
  std::map<std::string, Expression> propertyList;
  std::map<std::string, double> min_max_list;
  std::map<std::string, Expression> parameter_list;



};

/// Render expression to output stream
std::ostream & operator<<(std::ostream & out, const Expression & exp);

/// inequality comparison for two expressions (recursive)
bool operator!=(const Expression & left, const Expression & right) noexcept;
  
#endif
