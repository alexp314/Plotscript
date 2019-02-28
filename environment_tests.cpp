#include "catch.hpp"

#include "environment.hpp"
#include "semantic_error.hpp"

#include <cmath>

TEST_CASE( "Test default constructor", "[environment]" ) {

  Environment env;

  REQUIRE(env.is_known(Atom("pi")));
  REQUIRE(env.is_exp(Atom("pi")));

  REQUIRE(!env.is_known(Atom("hi")));
  REQUIRE(!env.is_exp(Atom("hi")));

  REQUIRE(env.is_proc(Atom("+")));
  REQUIRE(env.is_proc(Atom("-")));
  REQUIRE(env.is_proc(Atom("*")));
  REQUIRE(env.is_proc(Atom("/")));
  REQUIRE(!env.is_proc(Atom("op")));
}

TEST_CASE( "Test get expression", "[environment]" ) {
  Environment env;

  REQUIRE(env.get_exp(Atom("pi")) == Expression(std::atan2(0, -1)));
  REQUIRE(env.get_exp(Atom("hi")) == Expression());
}

TEST_CASE( "Test add expression", "[environment]" ) {
  Environment env;

  Expression a(Atom(1.0));
  env.add_exp(Atom("one"), a);
  REQUIRE(env.is_known(Atom("one")));
  REQUIRE(env.is_exp(Atom("one")));
  REQUIRE(env.get_exp(Atom("one")) == a);

  Expression b(Atom("hello"));
  env.add_exp(Atom("hi"), b);
  REQUIRE(env.is_known(Atom("hi")));
  REQUIRE(env.is_exp(Atom("hi")));
  REQUIRE(env.get_exp(Atom("hi")) == b);

  REQUIRE_THROWS_AS(env.add_exp(Atom(1.0), b), SemanticError);
}

TEST_CASE( "Test get built-in procedure", "[environment]" ) {
  Environment env;

  INFO("default procedure")
  Procedure p1 = env.get_proc(Atom("doesnotexist"));
  Procedure p2 = env.get_proc(Atom("alsodoesnotexist"));
  REQUIRE(p1 == p2);
  std::vector<Expression> args;
  REQUIRE(p1(args) == Expression());
  REQUIRE(p2(args) == Expression());

  INFO("trying add procedure")
  Procedure padd = env.get_proc(Atom("+"));
  args.emplace_back(1.0);
  args.emplace_back(2.0);
  REQUIRE(padd(args) == Expression(3.0));

  INFO("trying add procedure")
	  Procedure padd2 = env.get_proc(Atom("+"));
  args.emplace_back(Atom("asdf"));
  args.emplace_back(2.0);
  REQUIRE_THROWS_AS(padd2(args), SemanticError);



  

  INFO("trying mul procedure")
	  std::vector<Expression> args1;
	  Procedure mul = env.get_proc(Atom("*"));
  args1.emplace_back(Atom("asdfgh"));
  args1.emplace_back(std::complex<double>(2, 1));
  REQUIRE_THROWS_AS(mul(args1), SemanticError);




  INFO("trying subneg procedure")
	  std::vector<Expression> args2;
	  Procedure sub2 = env.get_proc(Atom("-"));
  args2.emplace_back(std::complex<double>(2, 1));
  args2.emplace_back(std::complex<double>(2, 1));
  args2.emplace_back(std::complex<double>(2, 1));
  REQUIRE_THROWS_AS(sub2(args2), SemanticError);




  INFO("trying div procedure")
	  Procedure div = env.get_proc(Atom("/"));
  args.emplace_back(std::complex<double>(2, 1));
  args.emplace_back(std::complex<double>(2, 1));
  args.emplace_back(std::complex<double>(2, 1));
  REQUIRE_THROWS_AS(div(args), SemanticError);




  INFO("trying sqrt procedure")
	  Procedure sq = env.get_proc(Atom("sqrt"));
  args.emplace_back(std::complex<double>(2, 1));
  args.emplace_back(std::complex<double>(2, 1));
  REQUIRE_THROWS_AS(sq(args), SemanticError);




  INFO("trying exponent procedure")
	  Procedure ex = env.get_proc(Atom("^"));
  args.emplace_back(std::complex<double>(2, 1));
  args.emplace_back(std::complex<double>(2, 1));
  args.emplace_back(std::complex<double>(2, 1));
  REQUIRE_THROWS_AS(ex(args), SemanticError);




  INFO("trying sine procedure")
	  Procedure sine = env.get_proc(Atom("sin"));
  args.emplace_back(std::complex<double>(2, 1));
  args.emplace_back(std::complex<double>(2, 1));
  REQUIRE_THROWS_AS(sine(args), SemanticError);



  INFO("trying ln procedure")
	  Procedure natL = env.get_proc(Atom("ln"));
  args.emplace_back(std::complex<double>(2, 1));
  args.emplace_back(std::complex<double>(2, 1));
  REQUIRE_THROWS_AS(natL(args), SemanticError);




  INFO("trying tangent procedure")
	  Procedure tangent = env.get_proc(Atom("tan"));
  args.emplace_back(std::complex<double>(2, 1));
  args.emplace_back(std::complex<double>(2, 1));
  REQUIRE_THROWS_AS(tangent(args), SemanticError);




  INFO("trying cosine procedure")
	  Procedure cosine = env.get_proc(Atom("cos"));
  args.emplace_back(std::complex<double>(2, 1));
  args.emplace_back(std::complex<double>(2, 1));
  REQUIRE_THROWS_AS(cosine(args), SemanticError);


  INFO("trying real part procedure")
	  std::vector<Expression> args3;
	  Procedure preal = env.get_proc(Atom("real"));
  args3.emplace_back(2);
  REQUIRE_THROWS_AS(preal(args3), SemanticError);

  INFO("trying real procedure")
	  std::vector<Expression> args4;
	  Procedure preal2 = env.get_proc(Atom("real"));
  args4.emplace_back(std::complex<double>(0, 1));
  args4.emplace_back(std::complex<double>(2, 1));
  REQUIRE_THROWS_AS(preal2(args4), SemanticError);




  INFO("trying imaginary part procedure")
	  std::vector<Expression> args5;
	  Procedure pimag = env.get_proc(Atom("imag"));
  args5.emplace_back(2);
  REQUIRE_THROWS_AS(pimag(args5), SemanticError);

  INFO("trying imaginary procedure")
	  std::vector<Expression> args6;
	  Procedure pimag2 = env.get_proc(Atom("imag"));
  args6.emplace_back(std::complex<double>(0, 1));
  args6.emplace_back(std::complex<double>(2, 1));
  REQUIRE_THROWS_AS(pimag2(args6), SemanticError);





  INFO("trying magnitude part procedure")
	  std::vector<Expression> args7;
  Procedure mag1 = env.get_proc(Atom("mag"));
  args7.emplace_back(2);
  REQUIRE_THROWS_AS(mag1(args7), SemanticError);

  INFO("trying magnitude procedure")
	  std::vector<Expression> args8;
	  Procedure mag = env.get_proc(Atom("mag"));
  args8.emplace_back(std::complex<double>(0, 1));
  args8.emplace_back(std::complex<double>(2, 1));
  REQUIRE_THROWS_AS(mag(args8), SemanticError);



  INFO("trying args part procedure")
	  std::vector<Expression> args9;
  Procedure arg1 = env.get_proc(Atom("arg"));
  args9.emplace_back(2);
  REQUIRE_THROWS_AS(arg1(args9), SemanticError);

  INFO("trying args procedure")
	  std::vector<Expression> args10;
	  Procedure arg = env.get_proc(Atom("arg"));
  args10.emplace_back(std::complex<double>(0, 1));
  args10.emplace_back(std::complex<double>(2, 1));
  REQUIRE_THROWS_AS(arg(args10), SemanticError);


  INFO("trying conj part procedure")
	  std::vector<Expression> args11;
  Procedure conj1 = env.get_proc(Atom("conj"));
  args11.emplace_back(2);
  REQUIRE_THROWS_AS(conj1(args11), SemanticError);

  INFO("trying conj procedure")
	  std::vector<Expression> args12;

	  Procedure conj = env.get_proc(Atom("conj"));
	  args12.emplace_back(std::complex<double>(0, 1));
	  args12.emplace_back(std::complex<double>(2, 1));
  REQUIRE_THROWS_AS(conj(args12), SemanticError);
}

TEST_CASE( "Test reset", "[environment]" ) {
  Environment env;

  Expression a(Atom(1.0));
  env.add_exp(Atom("one"), a);
  Expression b(Atom("hello"));
  env.add_exp(Atom("hi"), b);

  env.reset();
  REQUIRE(!env.is_known(Atom("one")));
  REQUIRE(!env.is_exp(Atom("one")));
  REQUIRE(env.get_exp(Atom("one")) == Expression());
  REQUIRE(!env.is_known(Atom("hi")));
  REQUIRE(!env.is_exp(Atom("hi")));
  REQUIRE(env.get_exp(Atom("hi")) == Expression());
}

TEST_CASE( "Test semeantic errors", "[environment]" ) {

  Environment env;

  {
    Expression exp(Atom("begin"));
    
    REQUIRE_THROWS_AS(exp.eval(env), SemanticError);
  }
}



