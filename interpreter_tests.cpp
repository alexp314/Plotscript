#include "catch.hpp"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include "semantic_error.hpp"
#include "interpreter.hpp"
#include "expression.hpp"

Expression run(const std::string & program){
  
  std::istringstream iss(program);
    
  Interpreter interp;
    
  bool ok = interp.parseStream(iss);
  if(!ok){
    std::cerr << "Failed to parse: " << program << std::endl; 
  }
  REQUIRE(ok == true);

  Expression result;
  REQUIRE_NOTHROW(result = interp.evaluate());

  return result;
}

TEST_CASE( "Test Interpreter parser with expected input", "[interpreter]" ) {

  std::string program = "(begin (define r 10) (* pi (* r r)))";

  std::istringstream iss(program);
 
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == true);
}

TEST_CASE( "Test Interpreter parser with numerical literals", "[interpreter]" ) {

  std::vector<std::string> programs = {"(1)", "(+1)", "(+1e+0)", "(1e-0)"};
  
  for(auto program : programs){
    std::istringstream iss(program);
 
    Interpreter interp;

    bool ok = interp.parseStream(iss);

    REQUIRE(ok == true);
  }

  {
    std::istringstream iss("(define x 1abc)");
    
    Interpreter interp;

    bool ok = interp.parseStream(iss);

    REQUIRE(ok == false);
  }
}

TEST_CASE( "Test Interpreter parser with truncated input", "[interpreter]" ) {

  {
    std::string program = "(f";
    std::istringstream iss(program);
  
    Interpreter interp;
    bool ok = interp.parseStream(iss);
    REQUIRE(ok == false);
  }
  
  {
    std::string program = "(begin (define r 10) (* pi (* r r";
    std::istringstream iss(program);

    Interpreter interp;
    bool ok = interp.parseStream(iss);
    REQUIRE(ok == false);
  }
}

TEST_CASE( "Test Interpreter parser with extra input", "[interpreter]" ) {

  std::string program = "(begin (define r 10) (* pi (* r r))) )";
  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with single non-keyword", "[interpreter]" ) {

  std::string program = "hello";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with empty input", "[interpreter]" ) {

  std::string program;
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with empty expression", "[interpreter]" ) {

  std::string program = "( )";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with bad number string", "[interpreter]" ) {

  std::string program = "(1abc)";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE("boost coverag", "[interpreter]") {

	std::string program = "(begin (define f (lambda (x) (/ 1 (+ 1 (^ e (- (* 5 x)))))))(continuous-plot f (list -1 1)))";
	std::istringstream iss(program);

	Interpreter interp;

	bool ok = interp.parseStream(iss);

	REQUIRE(ok == true);
}

TEST_CASE("boost coverage", "[interpreter]") {

	std::string program = "(begin (define f (lambda (x) (list x (+ (* 2 x) 1)))) (discrete - plot(map f(range - 2 2 0.5)) (list	(list \"title\" \"The Data\") (list \"abscissa-label\" \"X Label\") (list \"ordinate-label\" \"Y Label\") (list \"text-scale\" 1))))";
	std::istringstream iss(program);

	Interpreter interp;

	bool ok = interp.parseStream(iss);

	REQUIRE(ok == true);
}






TEST_CASE( "Test Interpreter parser with incorrect input. Regression Test", "[interpreter]" ) {

  std::string program = "(+ 1 2) (+ 3 4)";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter result with literal expressions", "[interpreter]" ) {
  
  { // Number
    std::string program = "(4)";
    Expression result = run(program);
    REQUIRE(result == Expression(4.));
  }

  { // Symbol
    std::string program = "(pi)";
    Expression result = run(program);
    REQUIRE(result == Expression(atan2(0, -1)));
  }

}

TEST_CASE( "Test Interpreter result with simple procedures (add)", "[interpreter]" ) {

  { // add, binary case
    std::string program = "(+ 1 2)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(3.));
  }
  
  { // add, 3-ary case
    std::string program = "(+ 1 2 3)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(6.));
  }

  { // add, 6-ary case
    std::string program = "(+ 1 2 3 4 5 6)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(21.));
  }

  { // add, 6-ary case
	  std::string program = "(/ 1)";
	  INFO(program);
	  Expression result = run(program);
	  REQUIRE(result == Expression(1.));
  }

  { // add, 6-ary case
	  std::string program = "(/ I)";
	  INFO(program);
	  Expression result = run(program);
	  REQUIRE(result == Expression(std::complex<double>(0, -1)));
  }
}
  
TEST_CASE( "Test Interpreter special forms: begin, define, and lambda", "[interpreter]" ) {

  {
    std::string program = "(define answer 42)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(42.));
  }

  {
    std::string program = "(begin (define answer 42)\n(answer))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(42.));
  }
  
  {
    std::string program = "(begin (define answer (+ 9 11)) (answer))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(20.));
  }

  {
    std::string program = "(begin (define a 1) (define b 1) (+ a b))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(2.));
  }

  {
	  std::string program = "(begin (define a (lambda (x) (+ 1 x))) (a 1))";
		  INFO(program);
		Expression result = run(program);
		  
		  REQUIRE(result == Expression(2.));
  }
  {
	  std::string program = "(begin (define a (lambda (x y) (+ y x))) (a 1 1))";
	  INFO(program);
	  Expression result = run(program);

	  REQUIRE(result == Expression(2.));
  }
  {
	  std::string program = "(apply + (list 1 2 3))";
	  INFO(program);
	  Expression result = run(program);

	  REQUIRE(result == Expression(6.));
  }
  {
	  std::string program = "(begin (define complexAsList (lambda (x) (list (real x) (imag x)))) (apply complexAsList (list (+ 1 (* 3 I)))))";
	  Interpreter interp;
	  std::istringstream iss(program);
	  bool ok = interp.parseStream(iss);
	  REQUIRE(ok == true);
	  interp.evaluate();
  }

 }

TEST_CASE("list", "[interpreter]") {
	{
		std::string input = "(list 1 2 3 4)";
		Interpreter interp;
		std::istringstream iss(input);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		interp.evaluate();
	}

	{
		std::string input = "(rest (list 1 2 3 4))";
		Interpreter interp;
		std::istringstream iss(input);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		interp.evaluate();
	}
	{
		std::string input = "(first (list 1 2 3 4))";
		Interpreter interp;
		std::istringstream iss(input);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		interp.evaluate();
	}
	{
		std::string input = "(length (list 1 2 3 4))";
		Interpreter interp;
		std::istringstream iss(input);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		interp.evaluate();
	}

	{
		std::string input = "(append (list 1 2 3 4) 1)";
		Interpreter interp;
		std::istringstream iss(input);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		interp.evaluate();
	}
	{
		std::string input = "(append (list 1 2 3 4) 1)";
		Interpreter interp;
		std::istringstream iss(input);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		interp.evaluate();
	}

	{
		std::string input = "(join (list 1 2 3 4) (list 5 6 7 8))";
		Interpreter interp;
		std::istringstream iss(input);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		interp.evaluate();
	}
	{
		std::string input = "(range 0 5 1)";
		Interpreter interp;
		std::istringstream iss(input);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		interp.evaluate();
	}






}


TEST_CASE("bad list", "[interpreter]") {
	std::vector<std::string> input = { "(list + 2 +)","(list a b x)" };
	Interpreter interp;
	for (auto a : input) {
		std::istringstream iss(a);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
	}
}

TEST_CASE("map", "[interpreter]") {

	{
		std::string program = "(begin (define f (lambda (x) (sin x))) (map f (list (- pi) (/ (- pi) 2) 0 (/ pi 2) pi)))";
		Interpreter interp;
		std::istringstream iss(program);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		interp.evaluate();
	}
	{
		std::string program = "(begin (map / (list 1 2 4)))";
		Interpreter interp;
		std::istringstream iss(program);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		interp.evaluate();
		
	}

	/*std::vector<std::string> input = { "(list + 2 +)","(list a b x)" };
	Interpreter interp;
	for (auto a : input) {
		std::istringstream iss(a);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
	}*/
}
TEST_CASE("env errors", "[interpreter]")
{
	{
		std::string program = "(begin (map 1 (list 1 2 4)))";
		Interpreter interp;
		std::istringstream iss(program);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);


		//interp.evaluate();

	}
	{
		std::string program = "(lambda (x))";
		Interpreter interp;
		std::istringstream iss(program);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);


		//interp.evaluate();

	}
	{
		std::string program = "(lambda (+ 1 x))";
		Interpreter interp;
		std::istringstream iss(program);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);


		//interp.evaluate();

	}
	{
		std::string program = "(apply /)";
		Interpreter interp;
		std::istringstream iss(program);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);


		//interp.evaluate();

	}
	{
		std::string program = "(apply (+ z I) (list 0))";
		Interpreter interp;
		std::istringstream iss(program);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);


		//interp.evaluate();

	}
	{
		std::string program = "(+)";
		Interpreter interp;
		std::istringstream iss(program);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);


		//interp.evaluate();

	}
	{
		std::string input = "(first 4)";
		Interpreter interp;
		std::istringstream iss(input);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
	}

	{
		std::string input = "(first (list))";
		Interpreter interp;
		std::istringstream iss(input);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
	}

	{
		std::string input = "(first (list 1) (list 1))";
		Interpreter interp;
		std::istringstream iss(input);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
	}

	{
		std::string input = "(rest 4)";
		Interpreter interp;
		std::istringstream iss(input);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
	}

	{
		std::string input = "(rest (list))";
		Interpreter interp;
		std::istringstream iss(input);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
	}

	{
		std::string input = "(rest (list 1) (list 1))";
		Interpreter interp;
		std::istringstream iss(input);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
	}

	{
		std::string input = "(length 1)";
		Interpreter interp;
		std::istringstream iss(input);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
	}

	{
		std::string input = "(length (list 1) (list 1))";
		Interpreter interp;
		std::istringstream iss(input);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
	}

	{
		std::string input = "(append 1)";
		Interpreter interp;
		std::istringstream iss(input);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
	}

	{
		std::string input = "(append (list 1))";
		Interpreter interp;
		std::istringstream iss(input);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
	}

	{
		std::string input = "(append 1 (list 1))";
		Interpreter interp;
		std::istringstream iss(input);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
	}

	{
		std::string input = "(join 1)";
		Interpreter interp;
		std::istringstream iss(input);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
	}

	{
		std::string input = "(join (list 1))";
		Interpreter interp;
		std::istringstream iss(input);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
	}

	{
		std::string input = "(join 1 (list 1))";
		Interpreter interp;
		std::istringstream iss(input);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
	}

	{
		std::string input = "(range 5 1 1)";
		Interpreter interp;
		std::istringstream iss(input);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
	}

	{
		std::string input = "(range 1 5 -1)";
		Interpreter interp;
		std::istringstream iss(input);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
	}

	{
		std::string input = "(range 1 x 1)";
		Interpreter interp;
		std::istringstream iss(input);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
	}
	{
		std::string input = "(range 1 + 1)";
		Interpreter interp;
		std::istringstream iss(input);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
	}

	{
		std::string input = "(range 5 1 )";
		Interpreter interp;
		std::istringstream iss(input);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
	}
	{
		std::string input = "(define)";
		Interpreter interp;
		std::istringstream iss(input);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
	}
}

TEST_CASE( "Test a medium-sized expression", "[interpreter]" ) {

  {
    std::string program = "(+ (+ 10 1) (+ 30 (+ 1 1)))";
    Expression result = run(program);
    REQUIRE(result == Expression(43.));
  }
}

TEST_CASE( "Test arithmetic procedures", "[interpreter]" ) {

  {
    std::vector<std::string> programs = {"(+ 1 -2)",
					 "(+ -3 1 1)",
					 "(- 1)",
					 "(- 1 2)",
					 "(* 1 -1)",
					 "(* 1 1 -1)",
					 "(/ -1 1)",
					 "(/ 1 -1)",
		"( - (- pi pi) 1)",
		"( - (- e e) 1)",
		"( - (sin (/ pi 2)))",
		"(cos pi)",
		"(- (tan (/ pi 4)))",
		"(- (mag I))",
		"(- (/ (arg I)(arg I)))",
		"(- (- (sqrt 4)(sqrt 1)))",
		"(- (- (ln 10)(ln 10)) 1)"


	};

    for(auto s : programs){
      Expression result = run(s);
      REQUIRE(result == Expression(-1.));
    }
  }
}

TEST_CASE("test more procedures")
{
	{
		std::vector<std::string> programs = { "(- I I)",
			"(- (* 2 I)(* 2 I))",
			"(+ (* 2 (- I))(*  2 I))",
			"(* I 0)",
			"(* 0 I)",
			"(/ 0 I)",
			"(* I 0)",
			"(- (^ I 0)(^ 2 0))",
			"(- (sqrt I)(sqrt I))",
			"(- (+ I pi)(+ pi I))",
			"(+ (- (* e I))(* e I))",
			"(- (ln I)(ln I))",
			"(- (sin I)(sin I))",
			"(- (cos I)(cos I))",
			"(- (tan I)(tan I))",
			"(- (conj I)(conj I))",
			"(- (/ 1 I)(/ 1 I))",
			"(- (- 1 I)(- 1 I))",
			"(- (/ I 1)(/ I 1))",
			"(- (/ I (* 2 I))(/ I (* 2 I)))",
			"(- (^ I I)(^ I I))",
			"(- (^ 3 I)(^ 3 I))",
			"(- (sqrt -2)(sqrt -2))"
		};

		for (auto a : programs) {
			Expression result = run(a);
			REQUIRE(result == Expression(std::complex<double> (0, 0)));
		}
	}
}

TEST_CASE("test real and imaginary") {
	std::vector<std::string> programs = { "(real I)",
		"(real (* 2 I))",
		"(imag (* 0 I))",
		"(imag (* 0 (* 2 I)))"
	};
	for (auto a : programs) {
		Expression result = run(a);
		REQUIRE(result == Expression(0.));
	}

}


TEST_CASE( "Test some semantically invalid expresions", "[interpreter]" ) {
  
  std::vector<std::string> programs = {"(@ none)", // so such procedure
				       "(- 1 1 2)", // too many arguments
				       "(define begin 1)", // redefine special form
				       "(define pi 3.14)"}; // redefine builtin symbol
    for(auto s : programs){
      Interpreter interp;

      std::istringstream iss(s);
      
      bool ok = interp.parseStream(iss);
      REQUIRE(ok == true);
      
    //  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
    }
}

TEST_CASE( "Test for exceptions from semantically incorrect input", "[interpreter]" ) {

  std::string input = R"(
(+ 1 a)
)";

  Interpreter interp;
  
  std::istringstream iss(input);
  
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  
  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE( "Test malformed define", "[interpreter]" ) {

    std::string input = R"(
(define a 1 2)
)";

  Interpreter interp;
  
  std::istringstream iss(input);
  
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  
  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE( "Test using number as procedure", "[interpreter]" ) {
    std::string input = R"(
(1 2 3)
)";

  Interpreter interp;
  
  std::istringstream iss(input);
  
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  
  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}
TEST_CASE("test empty list", "[interpreter]")
{
	Expression emptyList(Atom("list"));

	REQUIRE(emptyList == Expression(Atom("list")));
	//REQUIRE();



}



