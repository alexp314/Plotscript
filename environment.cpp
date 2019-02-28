#include "environment.hpp"

#include <cassert>
#include <cmath>
#include <complex>
#include <vector>

#include <iostream>

#include "environment.hpp"
#include "semantic_error.hpp"

/*********************************************************************** 
Helper Functions
**********************************************************************/

// predicate, the number of args is nargs
bool nargs_equal(const std::vector<Expression> & args, unsigned nargs){
  return args.size() == nargs;
};

Expression realPart(const std::vector<Expression> &args)
{
	if (nargs_equal(args, 1))
	{
		if (args[0].isHeadComplex())
		{
			return Expression(args[0].head().asComplex().real());
		}
		else
		{
			throw SemanticError("Error in call for real part: number not complex.");
		}
		
	}
	else
		throw SemanticError("Error in call for real part: invalid number of arguments");
};

Expression imagPart(const std::vector<Expression> &args)
{
	if (nargs_equal(args, 1))
	{
		if (args[0].isHeadComplex())
		{
			return Expression(args[0].head().asComplex().imag());
		}
		else
		{
			throw SemanticError("Error in call for imaginary part: number not complex.");
		}

	}
	else
		throw SemanticError("Error in call for imaginary part: invalid number of arguments");
};

Expression magnitude(const std::vector<Expression> &args)
{
	if (nargs_equal(args, 1))
	{
		if (args[0].isHeadComplex())
		{
			return Expression(abs(args[0].head().asComplex()));
		}
		else
		{
			throw SemanticError("Error in call for magnitude: number not complex.");
		}

	}
	else
		throw SemanticError("Error in call for magnitude: invalid number of arguments");
};

Expression anglePhase(const std::vector<Expression> &args)
{
	if (nargs_equal(args, 1))
	{
		if (args[0].isHeadComplex())
		{
			return Expression(arg(args[0].head().asComplex()));
		}
		else
		{
			throw SemanticError("Error in call for arg: number not complex.");
		}

	}
	else
		throw SemanticError("Error in call for arg: invalid number of arguments");
};

Expression conjugate(const std::vector<Expression> &args)
{
	if (nargs_equal(args, 1))
	{
		if (args[0].isHeadComplex())
		{
			return Expression(std::conj(args[0].head().asComplex()));
		}
		else
		{
			throw SemanticError("Error in call for conjugate: number not complex.");
		}

	}
	else
		throw SemanticError("Error in call for conjugate: invalid number of arguments");
};

/*********************************************************************** 
Each of the functions below have the signature that corresponds to the
typedef'd Procedure function pointer.
**********************************************************************/

// the default procedure always returns an expresison of type None
Expression default_proc(const std::vector<Expression> & args){
  args.size(); // make compiler happy we used this parameter
  return Expression();
}

Expression add(const std::vector<Expression> & args)
{

  // check all aruments are numbers, while adding
  std::complex<double> result;
  bool isC = false;
  if (args.size() != 0) {
	  for (auto & a : args) {
		  if (a.isHeadNumber()) {
			  result += a.head().asNumber();
		  }
		  else if (a.isHeadComplex()) {
			  isC = true;
			  result += a.head().asComplex();
		  }
		  else {
			  throw SemanticError("Error in call to add, argument not a number");
		  }
	  }
	  if (isC)
		  return Expression(result);
	  else
		  return Expression(result.real());
  }
  else
	  throw SemanticError("Error in add: no arguments input");
}

Expression mul(const std::vector<Expression> & args)
{
 
	// check all aruments are numbers, while multiplying
	Expression result;
	std::complex<double> c_result(1, 0);
	bool c_true = false;
		for (auto & a : args) {
			if (a.head().isComplex()) {
				c_true = true;
				c_result *= a.head().asComplex();

			}
			else if (a.isHeadNumber()) {
				c_result *= a.head().asNumber();

			}
			else {
				throw SemanticError("Error in call to mul, argument not a number");
			}
		}

		if (c_true)
			result = Expression(c_result);

		else if (!c_true)
			result = Expression(c_result.real());
	
		return result;

}

Expression subneg(const std::vector<Expression> & args)
{
	Expression result;
  std::complex<double> c_result (1, 0);
  bool isC = false;
  // preconditions
  if(nargs_equal(args,1)){
    if(args[0].isHeadNumber()){
      c_result = -args[0].head().asNumber();
    }
	else if (args[0].isHeadComplex()) {
		isC = true;
		c_result = -args[0].head().asComplex();
	}
   
  }
  else if(nargs_equal(args,2)){
    if( (args[0].isHeadNumber()) && (args[1].isHeadNumber()) ){
      c_result = args[0].head().asNumber() - args[1].head().asNumber();
    }
	else if (args[0].isHeadComplex() && args[1].isHeadNumber())
	{
	 isC = true;
		c_result = args[0].head().asComplex() - args[1].head().asNumber();
	}
	else if (args[0].isHeadNumber() && args[1].isHeadComplex())
	{
		isC = true;
		c_result = args[0].head().asNumber() - args[1].head().asComplex();
	}
	else if (args[0].isHeadComplex() && args[1].isHeadComplex())
	{
		isC = true;
		c_result = args[0].head().asComplex() - args[1].head().asComplex();
	}
  }
  else{
    throw SemanticError("Error in call to subtraction or negation: invalid number of arguments.");
  }
  if (isC) 
	  result = Expression(c_result);
  else if (!isC)
	  result = Expression(c_result.real());

  return result;
}

Expression div(const std::vector<Expression> & args)
{
	Expression exresult;
  std::complex<double> c_result (0, 0);
  double result = 0;  
  bool isC = false;
  if(nargs_equal(args,2))
  {
    if( (args[0].isHeadNumber()) && (args[1].isHeadNumber()) )
	{
      result = args[0].head().asNumber() / args[1].head().asNumber();
    }
	else if (args[0].isHeadComplex() || args[1].isHeadComplex()) 
	{
		isC = true;
		if (args[0].isHeadComplex() && args[1].isHeadNumber())
		{
			c_result = args[0].head().asComplex() / args[1].head().asNumber();
		}
		else if (args[0].isHeadNumber() && args[1].isHeadComplex())
		{
			c_result = args[0].head().asNumber() / args[1].head().asComplex();
		}
		else if (args[0].isHeadComplex() && args[1].isHeadComplex())
		{
			c_result = args[0].head().asComplex() / args[1].head().asComplex();
		}
	}
  }
  else if (nargs_equal(args, 1))
  {
	  if (args[0].isHeadNumber()) {
		  result = 1 / args[0].head().asNumber();
	  }
	  else if (args[0].isHeadComplex()) {
		  isC = true;
		  c_result = conj(args[0].head().asComplex());
	  }
  }
  else
  {
    throw SemanticError("Error in call to division: invalid number of arguments.");
  }
  if (isC)
	  exresult = Expression(c_result);
  else if (!isC)
	  exresult = Expression(result);

  return exresult;
}

Expression sqrt(const std::vector<Expression> & args)
{

	Expression eresult;
	std::complex<double> result (0, 0);
	std::complex<double> I(0, 1);
	bool isC = false;
	if (nargs_equal(args, 1))
	{
		if (args[0].isHeadNumber())
		{
			if (args[0].head().asNumber() >= 0) 
			{
				result = sqrt(args[0].head().asNumber());
			}
			else if (args[0].head().asNumber() < 0) 
			{
				isC = true;
				result = sqrt(abs(args[0].head().asNumber())) * I;
			}
		}
		else if (args[0].isHeadComplex()) {
			isC = true;
			result = sqrt(args[0].head().asComplex());
		}
	}
	else 
	{
		throw SemanticError("Error in call for square root: invalid number of arguments. ");
	}
	if (isC)
		eresult = Expression(result);
	else if (!isC)
		eresult = Expression(result.real());

	return eresult;
}

Expression exp(const std::vector<Expression> & args)
{
	Expression eresult;
	double result = 0;
	std::complex<double> c_result(0, 0);
	bool isC = false;
	if (nargs_equal(args, 2))
	{
		if ((args[0].isHeadNumber()) && (args[1].isHeadNumber()))
		{
			result = pow(args[0].head().asNumber(), args[1].head().asNumber());
		}
		else if ((args[0].isHeadNumber()) && (args[1].isHeadComplex()))
		{
			isC = true;
			c_result = pow(args[0].head().asNumber(), args[1].head().asComplex());
		}
		else if ((args[0].isHeadComplex()) && (args[1].isHeadNumber()))
		{
			isC = true;
			c_result = pow(args[0].head().asComplex(), args[1].head().asNumber());
		}
		else if ((args[0].isHeadComplex()) && (args[1].isHeadComplex()))
		{
			isC = true;
			c_result = pow(args[0].head().asComplex(), args[1].head().asComplex());
		}
	}
	else
	{
		throw SemanticError("Error in call to exponent: invalid number of arguments.");
	}
	if (isC)
		eresult = Expression(c_result);
	else if (!isC)
		eresult = Expression(result);

	return eresult;
}

Expression natLog(const std::vector<Expression> & args)
{
	Expression eresult;
	std::complex<double> result (0, 0);
	bool isC = false;
	if (nargs_equal(args, 1))
	{
		if (args[0].isHeadNumber())
		{
			result = log(args[0].head().asNumber());
		}
		else if (args[0].isHeadComplex()) {
			isC = true;
			result = log(args[0].head().asComplex());
		}
	}
	else
	{
		throw SemanticError("Error in call to ln: invalid number of arguments.");
	}
	if (isC)
		eresult = Expression(result);
	else if(!isC)
		eresult =  Expression(result.real());

	return eresult;
}

Expression sin(const std::vector<Expression> & args)
{
	Expression eresult;
	std::complex<double> result(0, 0);
	bool isC = false;
	if (nargs_equal(args, 1))
	{
		if (args[0].isHeadNumber())
		{
			result = sin(args[0].head().asNumber());
		}
		else if (args[0].isHeadComplex()) {
			isC = true;
			result = sin(args[0].head().asComplex());
		}	
	}
	else
	{
		throw SemanticError("Error in call to sin: invalid number of arguments.");
	}
	if (isC)
		eresult =  Expression(result);
	else if (!isC)
		eresult = Expression(result.real());

	return eresult;
}

Expression cos(const std::vector<Expression> & args)
{
	Expression eresult;
	std::complex<double> result(0, 0);
	bool isC = false;
	if (nargs_equal(args, 1))
	{
		if (args[0].isHeadNumber())
		{
			result = cos(args[0].head().asNumber());
		}
		else if (args[0].isHeadComplex()) {
			isC = true;
			result = cos(args[0].head().asComplex());
		}
	}
	else
	{
		throw SemanticError("Error in call to sin: invalid number of arguments.");
	}
	if (isC)
		eresult = Expression(result);
	else if (!isC)
		eresult =  Expression(result.real());

	return eresult;
}

Expression tan(const std::vector<Expression> & args)
{
	Expression eresult;
	std::complex<double> result(0, 0);
	bool isC = false;
	if (nargs_equal(args, 1))
	{
		if (args[0].isHeadNumber())
		{
			result = tan(args[0].head().asNumber());
		}
		else if (args[0].isHeadComplex()) {
			isC = true;
			result = tan(args[0].head().asComplex());
		}
		else
			throw SemanticError("Error in call to tan: invalid argument");
	}
	else
	{
		throw SemanticError("Error in call to sin: invalid number of arguments.");
	}
	if (isC)
		eresult = Expression(result);
	else if (!isC)
		eresult = Expression(result.real());

	return eresult;
}

Expression listFunction(const std::vector<Expression> & args)
{
	Atom HEAD("list");
	Expression myList {HEAD};
	//where did you come from where did you go, where did you come from counter-eyed-joe
	int counter = 0;//whaddafuqisdat
		for (auto & a : args){
			myList.appendExpression(a);
			counter++;//no idea what this is doing tbh but i'm scared to delete it
		}
	return  myList;

	///did you make a listKind?
	///then you need to set the head as a list
	///whadyameanthemilestoneisthehardestthingeverrrrr


}

Expression first(const std::vector<Expression> & args)
{
	Expression myList;
	if (nargs_equal(args, 1)){
		if (args[0].tailConstBegin() != args[0].tailConstEnd()) {
			if (args[0].head().asSymbol() == "list")
				myList = *(args[0].tailConstBegin());
		}
		else
			throw SemanticError("Error in call to first: empty list");
	}
	else
		throw SemanticError("Error in call to first: incorrect number of arguments");
	return myList;
}

Expression rest(const std::vector<Expression> & args)
{
	//std::cout << args.size() << " size" << std::endl;
	Atom HEAD("rest");
	Expression myList{HEAD};
	if (nargs_equal(args, 1)) {
		if (args[0].head().asSymbol() == "list") {
			if (args[0].tailConstBegin() != args[0].tailConstEnd()) {
				for (auto e = args[0].tailConstBegin() + 1; e != args[0].tailConstEnd(); e++) {
					myList.appendExpression(*e);
				}
			}
			else
				throw SemanticError("Error in call to rest: empty list");
		}
		else
			throw SemanticError("Error in call for rest: not a list list");
	}
	else
		throw SemanticError("Error in call to rest: incorrect number of arguments");
	return myList;
}

Expression length(const std::vector<Expression> & args)
{
	Atom HEAD("length");
	Expression myLength(HEAD);
	double length = 0;
	if (nargs_equal(args, 1)) {
		//if (args[0].tailConstBegin() != args[0].tailConstEnd()) {
		
			if (args[0].head().asSymbol() == "list") {
				for (auto e = args[0].tailConstBegin(); e != args[0].tailConstEnd(); e++) {
					length++;
				}
				myLength.append(length);
			}
		//}
		else
			throw SemanticError("Error in call for length: argument not a list");
	}
	else
		throw SemanticError("Error in call to length: incorrect number of arguments");
	return myLength;
}

Expression append(const std::vector<Expression> & args)
{
	Atom HEAD("list");
	Expression appList{ HEAD };
	if (nargs_equal(args, 2)) {
		if (args[0].tailConstBegin() != args[0].tailConstEnd()) {
			if (args[0].head().asSymbol() == "list") {
				for (auto e = args[0].tailConstBegin(); e != args[0].tailConstEnd(); e++) {
					appList.appendExpression(*e);
				}
			}
			if (args[1].head().asSymbol() == "list") {
				for (auto e = args[1].tailConstBegin(); e != args[1].tailConstEnd(); e++) {
					appList.appendExpression(*e);
				}
			}
			else
				appList.appendExpression(args[1]);
		}
		else
			throw SemanticError("Error in call to append: first argument not a list");
	}
	else
		throw SemanticError("Error in call to append: incorrect number of arguments");
	return appList;
}

Expression join(const std::vector<Expression> & args)
{
	Atom HEAD("list");
	Expression joinList{ HEAD };
	if (nargs_equal(args, 2)) {
		if (args[0].head().asSymbol() == "list" && args[1].head().asSymbol() == "list") {
			for (auto e = args[0].tailConstBegin(); e != args[0].tailConstEnd(); e++) {
				joinList.appendExpression(*e);
			}
			for (auto e = args[1].tailConstBegin(); e != args[1].tailConstEnd(); e++) {
				joinList.appendExpression(*e);
			}
		}
		else
			throw SemanticError("Error in call to append: argument not a list");
	}
	else
		throw SemanticError("Error in call to join: incorrect number of arguments");
	return joinList;
}

Expression range(const std::vector<Expression> & args)
{
	Atom HEAD("list");
	Expression rangeList{ HEAD };
	if (nargs_equal(args, 3)) {
		double first = args[0].head().asNumber();
		double second = args[1].head().asNumber();
		double third = args[2].head().asNumber();
		if (args[0].isHeadNumber() && args[1].isHeadNumber() && args[2].isHeadNumber()) {
			if (args[0].head().asNumber() < args[1].head().asNumber()) {
				if (args[2].head().asNumber() > 0) {
					double i = first;
					while (i <= second) {
						rangeList.append(i);
						i = i + third;
					}
				}
				else
					throw SemanticError("Error in call to range: negative or zero increment in range");
			}
			else
				throw SemanticError("Error in call to range: begin greater than end");
		}
		else
			throw SemanticError("Error in call to range: argument is not a number");
	}
	else
		throw SemanticError("Error in call to range: incorrect number of arguments");
	return rangeList;
}

/*Expression setProperty(const std::vector<Expression> & args)
{
	Expression value;
	if (nargs_equal(args, 3)) {
		if (args[0].head().isSymbol() && args[0].head().asSymbol().at(0) != '"')
			throw SemanticError("Error in set-property: first argument not a list");
		if (!args[0].head().isSymbol())
			throw SemanticError("Error: first argument to set-property not a string");

		if (args[0].head().asSymbol().at(0) == '"')
		{
			add_exp();

			//localMap.emplace("pi", EnvResult(ExpressionType, Expression(PI)));
		}

	}
	else
		throw SemanticError("Error: incorrect number of arguments in set-property");

	return value;
}*/


Expression discrete_plot(const std::vector<Expression> & args)
{
	return args[0];
}

const std::complex<double> I(0, 1);
const double PI = std::atan2(0, -1);
const double EXP = std::exp(1);

Environment::Environment(){

  reset();
}

bool Environment::is_known(const Atom & sym) const{
  if(!sym.isSymbol()) return false;
  
  return envmap.find(sym.asSymbol()) != envmap.end();
}

bool Environment::is_exp(const Atom & sym) const{
  if(!sym.isSymbol()) return false;
  
  auto result = envmap.find(sym.asSymbol());
  return (result != envmap.end()) && (result->second.type == ExpressionType);
}

Expression Environment::get_exp(const Atom & sym) const{

  Expression exp;
  
  if(sym.isSymbol()){
    auto result = envmap.find(sym.asSymbol());
    if((result != envmap.end()) && (result->second.type == ExpressionType)){
      exp = result->second.exp;
    }
  }

  return exp;
}

void Environment::add_exp(const Atom & sym, const Expression & exp){

  if(!sym.isSymbol()){
    throw SemanticError("Attempt to add non-symbol to environment");
  }
    
  if (sym.asSymbol() == "set-property")
  {
	  throw SemanticError("error");
  }
  /// error if overwriting symbol map
  if(envmap.find(sym.asSymbol()) != envmap.end()){
	  envmap[sym.asSymbol()] = EnvResult(ExpressionType, exp);
  }

  envmap.emplace(sym.asSymbol(), EnvResult(ExpressionType, exp)); 
}

bool Environment::is_proc(const Atom & sym) const{
  if(!sym.isSymbol()) return false;
  
  auto result = envmap.find(sym.asSymbol());
  return (result != envmap.end()) && (result->second.type == ProcedureType);
}


Procedure Environment::get_proc(const Atom & sym) const{

  //Procedure proc = default_proc;

  if(sym.isSymbol()){
    auto result = envmap.find(sym.asSymbol());
    if((result != envmap.end()) && (result->second.type == ProcedureType)){
      return result->second.proc;
    }
  }

  return default_proc;
}




/*
Reset the environment to the default state. First remove all entries and
then re-add the default ones.
 */
void Environment::reset(){

  envmap.clear();
  
  // Built-In value of pi
  envmap.emplace("pi", EnvResult(ExpressionType, Expression(PI)));

  // Built-In value of e
  envmap.emplace("e", EnvResult(ExpressionType, Expression(EXP)));

  // Built-In value of i
  envmap.emplace("I", EnvResult(ExpressionType, Expression(I)));

  // Procedure: add;
  envmap.emplace("+", EnvResult(ProcedureType, add)); 

  // Procedure: subneg;
  envmap.emplace("-", EnvResult(ProcedureType, subneg)); 
	
  // Procedure: mul;
  envmap.emplace("*", EnvResult(ProcedureType, mul)); 

  // Procedure: div;
  envmap.emplace("/", EnvResult(ProcedureType, div));

  //Procedure: sqrt;
  envmap.emplace("sqrt", EnvResult(ProcedureType, sqrt));
  
  //Procedure: exp;
  envmap.emplace("^", EnvResult(ProcedureType, exp));

  //Procedure: ln;
  envmap.emplace("ln", EnvResult(ProcedureType, natLog));

  //Procedure: sin;
  envmap.emplace("sin", EnvResult(ProcedureType, sin));

  //Procedure: cos;
  envmap.emplace("cos", EnvResult(ProcedureType, cos));

  //Procedure: tan;
  envmap.emplace("tan", EnvResult(ProcedureType, tan));

  //Procedure: real;
  envmap.emplace("real", EnvResult(ProcedureType, realPart));

  //Procedure: imaginary;
  envmap.emplace("imag", EnvResult(ProcedureType, imagPart));

  //Procedure: magnitude;
  envmap.emplace("mag", EnvResult(ProcedureType, magnitude));

  //Procedure: phase/angle;
  envmap.emplace("arg", EnvResult(ProcedureType, anglePhase));

  //Procedure: conjugate;
  envmap.emplace("conj", EnvResult(ProcedureType, conjugate));

  //Procedure: list;
  envmap.emplace("list", EnvResult(ProcedureType, listFunction));

  //Procedure: first;
  envmap.emplace("first", EnvResult(ProcedureType, first));

  //Procedure: rest;
  envmap.emplace("rest", EnvResult(ProcedureType, rest));

  //Procedure: length;
  envmap.emplace("length", EnvResult(ProcedureType, length));

  //Procedure: append;
  envmap.emplace("append", EnvResult(ProcedureType, append));

  //Procedure: join;
  envmap.emplace("join", EnvResult(ProcedureType, join));

  //Procedure: range;
  envmap.emplace("range", EnvResult(ProcedureType, range));

  //Procedure: discrete plot;
  envmap.emplace("discrete-plot", EnvResult(ProcedureType, discrete_plot));
  //Procedure: set-property;
  //envmap.emplace("set-property", EnvResult(ProcedureType, setProperty));

  //Procedure: get-property;
//  envmap.emplace("get-property", EnvResult(ProcedureType, getProperty));
}
