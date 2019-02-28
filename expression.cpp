#include "expression.hpp"
#include "environment.hpp"
#include <string>
#include <iomanip>

#include <sstream>
#include <list>
#include <vector>

#include <iostream>

#include "environment.hpp"
#include "semantic_error.hpp"

const double N = 20;
const double A = 3;
const double B = 3;
const double C = 2;
const double D = 2;
const double P = 0.5;
const double PI = std::atan2(0, -1);



Expression::Expression() {}

Expression::Expression(const Atom & a) {

	m_head = a;
}

// recursive copy

Expression::Expression(const Expression & a) {

	m_head = a.m_head;
	for (auto e : a.m_tail) {
		m_tail.push_back(e);
	}
	propertyList = a.propertyList;

}

Expression & Expression::operator=(const Expression & a) {

	// prevent self-assignment
	if (this != &a) {
		m_head = a.m_head;
		m_tail.clear();
		for (auto e : a.m_tail) {
			m_tail.push_back(e);
		}
		propertyList = a.propertyList;
	}

	return *this;
}


Atom & Expression::head() {
	return m_head;
}

const Atom & Expression::head() const {
	return m_head;
}

bool Expression::isHeadNumber() const noexcept {
	return m_head.isNumber();
}

bool Expression::isHeadSymbol() const noexcept {
	return m_head.isSymbol();
}

bool Expression::isHeadComplex() const noexcept
{
	return m_head.isComplex();
}

bool Expression::map_contains(std::string key)
{
	if (parameter_list.find(key) == parameter_list.cend())
		return false;
	else
		return true;
}

void Expression::append(const Atom & a) {
	m_tail.emplace_back(a);
}

void Expression::appendExpression(const Expression & a) {
	m_tail.push_back(a);
}


Expression * Expression::tail() {
	Expression * ptr = nullptr;

	if (m_tail.size() > 0) {
		ptr = &m_tail.back();
	}

	return ptr;
}

Expression::ConstIteratorType Expression::tailConstBegin() const noexcept {
	return m_tail.cbegin();
}

Expression::ConstIteratorType Expression::tailConstEnd() const noexcept {
	return m_tail.cend();
}

//bool Expression::isList()
Expression apply(const Atom & op, const std::vector<Expression> & args, const Environment & env) {
	Environment lambdaEnv = env;
	Expression exp;




	// head must be a symbol
	if (!op.isSymbol()) {
		throw SemanticError("Error during evaluation: procedure name not symbol");
	}

	// must map to a proc
	if (!env.is_proc(op)) {
		throw SemanticError("Error during evaluation: symbol does not name a procedure");
	}

	// map from symbol to proc
	Procedure proc = env.get_proc(op);

	// call proc with args
	return proc(args);
}

Expression Expression::handle_lookup(const Atom & head, const Environment & env) {

	//this;


	if (head.isSymbol() && head.asSymbol().at(0) != '"') { // if symbol is in env return value
		if (env.is_exp(head)) {
			return env.get_exp(head);
		}
		else {
			throw SemanticError("Error during evaluation: unknown symbol");
		}
	}
	else if (head.isNumber() || head.isComplex() || head.asSymbol().at(0) == '"') {
		return Expression(head);
	}
	else {
		throw SemanticError("Error during evaluation: Invalid type in terminal expression");
	}
}

Expression Expression::handle_begin(Environment & env) {

	if (m_tail.size() == 0) {
		throw SemanticError("Error during evaluation: zero arguments to begin");
	}
	// evaluate each arg from tail, return the last
	Expression result;
	for (Expression::IteratorType it = m_tail.begin(); it != m_tail.end(); ++it) {
		result = it->eval(env);
	}
	return result;
}

Expression Expression::handle_lambda(Environment & env)
{
	if (m_tail.size() != 2)
		throw SemanticError("Error during lambda evaluation: invalid number of arguments");
	Expression result = *this;
	Atom HEAD("list");
	Expression myList{ HEAD };
	std::vector<Expression> list;
	list.emplace_back(result.m_tail[0].head());
	for (unsigned int i = 0; i < result.m_tail[0].m_tail.size(); i++) {
		list.emplace_back(result.m_tail[0].m_tail[i]);
	}
	myList = listFunction(list);
	result.m_tail[0] = myList;
	env.is_known(m_head);
	//if (result.m_tail[0].head().asSymbol() != "list")
	//throw SemanticError("Error during lambda evaluation: first argument not a list");

	return result;
}


Expression Expression::handle_define(Environment & env) {

	// tail must have size 3 or error
	if (m_tail.size() != 2) {
		throw SemanticError("Error during evaluation: invalid number of arguments to define");
	}
	// tail[0] must be symbol
	if (!m_tail[0].isHeadSymbol()) {
		throw SemanticError("Error during evaluation: first argument to define not symbol");
	}
	if (env.is_proc(m_tail[0].m_head))
		throw SemanticError("Error: attempt to redefine procedure");
	std::string s = m_tail[0].head().asSymbol();
	if ((s == "define") || (s == "begin")) {
		throw SemanticError("Error during evaluation: attempt to redefine a special-form");
	}
	if (env.is_proc(m_head)) {
		throw SemanticError("Error during evaluation: attempt to redefine a built-in procedure");
	}
	Expression result = m_tail[1].eval(env);
	//and add to env
	env.add_exp(m_tail[0].head(), result);
	return result;
}

Expression Expression::doApply(Environment& env)
{
	Expression exp = env.get_exp(m_tail[0].m_head);
	Expression result;
	std::vector<Expression> args;
	if (m_tail.size() != 2)
		throw SemanticError("Error in call to apply: incorrect number of arguments");
	if (m_tail[0].m_tail.size() != 0 || !env.is_known(m_tail[0].m_head))
		throw SemanticError("Error in call to apply: first argument not a procedure");
	if (m_tail[1].head().asSymbol() != "list")
		throw SemanticError("Error in call to apply: second argument not a list");
	if (env.is_proc(m_tail[0].m_head))
	{
		///pass to apply
		for (unsigned int i = 0; i < m_tail[1].m_tail.size(); i++) {
			args.emplace_back(m_tail[1].m_tail[i]);
		}
		result = apply(m_tail[0].m_head, args, env);
	}
	else if (env.is_exp(m_tail[0].m_head))
	{
		if (exp.head().asSymbol() == "lambda")
		{
			Expression branchEval(m_tail[1]);
			Expression evalList = branchEval.eval(env);
			Expression lambdaTree(m_tail[0].m_head.asSymbol());
			for (unsigned int i = 0; i < evalList.m_tail.size(); i++)
			{
				lambdaTree.appendExpression(evalList.m_tail[i]);
			}
			result = lambdaTree.eval(env);
		}
	}
	return result;
}

Expression Expression::doMap(Environment& env, Environment& lambdaEnv) {
	//Expression exp = env.get_exp(m_tail[0].m_head);

	Expression result;
	Expression check_list = m_tail[1].eval(env);
	if (m_tail.size() != 2)
		throw SemanticError("Error in call for map: incorrect number of arguments");
	//if (!env.is_proc(m_tail[0].m_head))
	// throw SemanticError("Error in call for map: first argument not a procedure");
	if (m_tail[0].m_tail.size() != 0 || m_tail[0].m_head.isNumber())
		throw SemanticError("Error incall for map: first argument not a procedure");
	if (check_list.m_head.asSymbol() != "list")
		throw SemanticError("Error in call for map: second argument not a list");
	Expression exp = env.get_exp(m_tail[0].m_head);

	if (env.is_proc(m_tail[0].m_head))
	{
		Expression mapList(Atom("list"));
		for (unsigned int i = 0; i < check_list.m_tail.size(); i++)
		{
			Expression mapExp(m_tail[0].m_head);
			mapExp.appendExpression(check_list.m_tail[i]);
			mapList.appendExpression(mapExp.eval(env));

		}
		result = mapList;
	}
	else if (env.is_exp(m_tail[0].m_head))
	{
		Expression mapLambda(Atom("list"));
		if (exp.head().asSymbol() == "lambda")
		{
			//Expression mapLambda;
			for (unsigned int i = 0; i < check_list.m_tail.size(); i++) {
				Expression lambdaMapTree(m_tail[0].m_head);
				lambdaMapTree.appendExpression(check_list.m_tail[i]);
				mapLambda.appendExpression(lambdaMapTree.eval(lambdaEnv));

			}
			result = mapLambda;

		}
	}
	return result;
}

Expression Expression::doLambda(Environment & lambdaEnv, Environment & env, Expression check)
{
	for (unsigned int i = 0; i < m_tail.size(); i++)
	{
		Expression exp(Atom("define"));
		exp.appendExpression(check.m_tail[0].m_tail[i]);
		exp.appendExpression(m_tail[i]);
		exp.eval(lambdaEnv);
		env.is_known(m_head);
	}
	Expression LAMBDA(check.m_tail[1]);
	return LAMBDA.eval(lambdaEnv);
}

Expression Expression::doSetProperty(Environment& env)
{

	if (m_tail[0].isHeadSymbol() && m_tail[0].head().asSymbol().at(0) != '"')
		throw SemanticError("Error: first property not a string");
	else if (m_tail.size() != 3)
		throw SemanticError("Error: incorrect number of arguments");

	Expression propertyName = m_tail[0].eval(env);
	Expression propertyO = m_tail[1].eval(env);
	Expression hasproperty = m_tail[2].eval(env);

	hasproperty.propertyList[propertyName.head().asSymbol()] = propertyO;

	return hasproperty;
}

Expression Expression::doGetProperty(Environment & env)
{

	//Environment envi = env;
	if (m_tail[0].isHeadSymbol() && m_tail[0].head().asSymbol().at(0) != '"')
		throw SemanticError("Error: first property not a string");
	if (m_tail.size() != 2)
		throw SemanticError("Error: incorrect number of arguments");
	Expression propName = m_tail[0];
	Expression propLoc = env.get_exp(m_tail[1].head());

	Expression hasProperty = propLoc.propertyList[propName.head().asSymbol()];
	return hasProperty;
}
Expression Expression::make_plot_bound(Environment &env, Expression origin_head, Expression &output)
{
	Environment envi = env;
	Expression org = origin_head;
	Expression bound(Atom("list"));
	Expression objectname(Atom("\"object-name\""));
	Expression propnamepoint(Atom("\"point\""));
	Expression propnameline(Atom("\"line\""));

	double x_min = min_max_list["x_min"];
	double x_max = min_max_list["x_max"];
	double y_min = min_max_list["y_min"];
	double y_max = min_max_list["y_max"];
	double x_scale = min_max_list["x_scale"];
	double y_scale = min_max_list["y_scale"];

	//bottom left point coords
	Expression bottom_left_x(Atom(x_scale * x_min));
	Expression bottom_left_y(Atom(-(y_scale * y_min)));

	//bottom right point coords
	Expression bottom_right_x(Atom(x_scale * x_max));
	Expression bottom_right_y(Atom(-(y_scale * y_min)));

	//top left point coords
	Expression top_left_x(Atom(x_scale * x_min));
	Expression top_left_y(Atom(-(y_scale * y_max)));

	//top right point coords
	Expression top_right_x(Atom(x_scale * x_max));
	Expression top_right_y(Atom(-(y_scale * y_max)));


	//bottom left point
	Expression bottom_left_point(Atom("list"));
	bottom_left_point.appendExpression(bottom_left_x);
	bottom_left_point.appendExpression(bottom_left_y);
	bottom_left_point.propertyList[objectname.head().asSymbol()] = propnamepoint;

	//bottom right point
	Expression bottom_right_point(Atom("list"));
	bottom_right_point.appendExpression(bottom_right_x);
	bottom_right_point.appendExpression(bottom_right_y);
	bottom_right_point.propertyList[objectname.head().asSymbol()] = propnamepoint;

	//top left point 
	Expression top_left_point(Atom("list"));
	top_left_point.appendExpression(top_left_x);
	top_left_point.appendExpression(top_left_y);
	top_left_point.propertyList[objectname.head().asSymbol()] = propnamepoint;

	//top right point
	Expression top_right_point(Atom("list"));
	top_right_point.appendExpression(top_right_x);
	top_right_point.appendExpression(top_right_y);
	top_right_point.propertyList[objectname.head().asSymbol()] = propnamepoint;

	//bottom line
	Expression bottom(Atom("list"));
	bottom.appendExpression(bottom_left_point);
	bottom.appendExpression(bottom_right_point);
	bottom.propertyList[objectname.head().asSymbol()] = propnameline;

	//top line
	Expression top(Atom("list"));
	top.appendExpression(top_left_point);
	top.appendExpression(top_right_point);
	top.propertyList[objectname.head().asSymbol()] = propnameline;

	//left line
	Expression left(Atom("list"));
	left.appendExpression(bottom_left_point);
	left.appendExpression(top_left_point);
	left.propertyList[objectname.head().asSymbol()] = propnameline;

	//right line
	Expression right(Atom("list"));
	right.appendExpression(bottom_right_point);
	right.appendExpression(top_right_point);
	right.propertyList[objectname.head().asSymbol()] = propnameline;


	output.appendExpression(top);
	output.appendExpression(bottom);
	output.appendExpression(left);
	output.appendExpression(right);



	return bound;
}

void Expression::get_min_max(Expression tail_0, Expression tail_1)
{
	/*Expression min_x(Atom("x_min"));
	Expression max_x(Atom("x_max"));
	Expression min_y(Atom("y_min"));
	Expression max_y(Atom("y_max"));*/
	Expression ex = tail_1;
		double x_max = tail_0.m_tail[0].m_tail[0].head().asNumber();
		double x_min = tail_0.m_tail[0].m_tail[0].head().asNumber();
		double y_max = tail_0.m_tail[0].m_tail[1].head().asNumber();
		double y_min = tail_0.m_tail[0].m_tail[1].head().asNumber();

		for (unsigned int i = 0; i < tail_0.m_tail.size(); i++)
		{
			if (tail_0.m_tail[i].m_tail[0].head().asNumber() > x_max)
				x_max = tail_0.m_tail[i].m_tail[0].head().asNumber();

			else if (tail_0.m_tail[i].m_tail[0].head().asNumber() < x_min)
				x_min = tail_0.m_tail[i].m_tail[0].head().asNumber();

			if (tail_0.m_tail[i].m_tail[1].head().asNumber() > y_max)
				y_max = tail_0.m_tail[i].m_tail[1].head().asNumber();

			else if (tail_0.m_tail[i].m_tail[1].head().asNumber() < y_min)
				y_min = tail_0.m_tail[i].m_tail[1].head().asNumber();
		}


		min_max_list["x_min"] = x_min;
		min_max_list["x_max"] = x_max;
		min_max_list["y_min"] = y_min;
		min_max_list["y_max"] = y_max;
		min_max_list["x_scale"] = 20.0 / (x_max - x_min);
		min_max_list["y_scale"] = 20.0 / (y_max - y_min);

	/*std::cout << "xmin: " << x_min << " xmax: " << x_max << " ymin: "
	<< y_min << " ymax: " << y_max;*/
}
Expression Expression::get_labels(Expression tail_1, Expression & output, int plot)
{
	Expression lab;
	//int p = plot;
	plot = 25;
	Expression ex(plot);
	Expression objectname(Atom("\"object-name\""));
	Expression propnametext(Atom("\"text\""));
	double x_min = min_max_list["x_min"];
	double x_max = min_max_list["x_max"];
	double y_min = min_max_list["y_min"];
	double y_max = min_max_list["y_max"];
	double x_scale = min_max_list["x_scale"];
	double y_scale = min_max_list["y_scale"];


	Expression objectposition(Atom("\"position\""));

	//position.propertyList[objectposition.head().asSymbol()] = position;

	Expression objectscale(Atom("\"text-scale\""));



	Expression objectrotation(Atom("\"text-rotation\""));

	std::ostringstream oss;
	oss << std::setprecision(2) << y_max;
	std::string O_U = oss.str();
	Expression OU(O_U);
	Expression title;
	Expression abscissa_label;
	Expression ordinate_label;

	std::ostringstream oss1;

	oss1 << std::setprecision(2) << y_min;
	std::string O_L = oss1.str();
	Expression OL(O_L);

	std::ostringstream oss2;

	oss2 << std::setprecision(2) << x_min;
	std::string A_L = oss2.str();
	Expression AL(A_L);

	std::ostringstream oss3;

	oss3 << std::setprecision(2) << x_max;
	std::string A_U = oss3.str();
	Expression AU(A_U);
	//Expression title;
	//Expression abscissa_label;
	//Expression ordinate_label;

	if (tail_1.head().asSymbol() != "empty")
	{
		for (unsigned int i = 0; i < tail_1.m_tail.size(); i++)
		{
			//parameter_list[tail_1.m_tail[i].m_tail[0].head().asSymbol()] = tail_1.m_tail[i].m_tail[1];
			if (tail_1.m_tail[i].m_tail[0].head().asSymbol() == "\"title\"")
			{
				Expression position(Atom("list"));
				Expression y(Atom(-((y_max* y_scale) + 3)));
				Expression x(Atom((x_max*x_scale) - (((x_max*x_scale) - (x_min *x_scale)) / 2)));
				position.appendExpression(x);
				position.appendExpression(y);
				Expression scale(Atom(1));
				Expression titlerotation(Atom(0));

				title = tail_1.m_tail[i].m_tail[1];
				title.propertyList[objectname.head().asSymbol()] = propnametext;
				title.propertyList[objectposition.head().asSymbol()] = position;
				title.propertyList[objectscale.head().asSymbol()] = scale;
				title.propertyList[objectrotation.head().asSymbol()] = titlerotation;

			}
			else if (tail_1.m_tail[i].m_tail[0].head().asSymbol() == "\"ordinate-label\"")
			{
				Expression position(Atom("list"));

				Expression y(Atom(-((y_max*y_scale) - (((y_max * y_scale) - (y_min * y_scale)) / 2))));
				//Expression y(Atom(((y_max *y_scale) - (y_min *y_scale))/2));
				//double x_param = (x_max * x_scale)
				Expression x(Atom(((x_min * x_scale) - 3)));
				position.appendExpression(x);
				position.appendExpression(y);
				Expression scale(Atom(1));
				//Expression titlerotation(Atom(0));

				Expression titlerotation(Atom(-PI / 2));
				abscissa_label = tail_1.m_tail[i].m_tail[1];
				abscissa_label.propertyList[objectname.head().asSymbol()] = propnametext;
				abscissa_label.propertyList[objectposition.head().asSymbol()] = position;
				abscissa_label.propertyList[objectscale.head().asSymbol()] = scale;
				abscissa_label.propertyList[objectrotation.head().asSymbol()] = titlerotation;

			}
			else if (tail_1.m_tail[i].m_tail[0].head().asSymbol() == "\"abscissa-label\"")
			{
				Expression position(Atom("list"));
				Expression y(Atom(-((y_min* y_scale) - 3)));
				//double x_param = (x_max * x_scale)
				Expression x(Atom((x_max*x_scale) - (((x_max*x_scale) - (x_min *x_scale)) / 2)));
				position.appendExpression(x);
				position.appendExpression(y);
				Expression scale(Atom(1));
				//Expression titlerotation(Atom(0));

				Expression titlerotation(Atom(0));
				ordinate_label = tail_1.m_tail[i].m_tail[1];
				ordinate_label.propertyList[objectname.head().asSymbol()] = propnametext;
				ordinate_label.propertyList[objectposition.head().asSymbol()] = position;
				ordinate_label.propertyList[objectscale.head().asSymbol()] = scale;
				ordinate_label.propertyList[objectrotation.head().asSymbol()] = titlerotation;

			}
			else if (tail_1.m_tail[i].m_tail[0].head().asSymbol() == "\"text-scale\"")
			{

				Expression scale = tail_1.m_tail[i].m_tail[1];
				ordinate_label.propertyList[objectscale.head().asSymbol()] = scale;
				abscissa_label.propertyList[objectscale.head().asSymbol()] = scale;
				title.propertyList[objectscale.head().asSymbol()] = scale;
			}

		}
		output.appendExpression(title);
		output.appendExpression(abscissa_label);
		output.appendExpression(ordinate_label);
	}

	//OU
	Expression position(Atom("list"));
	Expression y(Atom(-(y_max * y_scale)));
	Expression x(Atom((x_min * x_scale) - 2));
	position.appendExpression(x);
	position.appendExpression(y);
	Expression scale(Atom(1));
	Expression titlerotation(Atom(0));
	OU.propertyList[objectname.head().asSymbol()] = propnametext;
	OU.propertyList[objectposition.head().asSymbol()] = position;
	OU.propertyList[objectscale.head().asSymbol()] = scale;
	OU.propertyList[objectrotation.head().asSymbol()] = titlerotation;

	//ol
	Expression position2(Atom("list"));
	Expression y1(Atom(-(y_min * y_scale)));
	Expression x1(Atom((x_min * x_scale) - 2));
	position2.appendExpression(x1);
	position2.appendExpression(y1);
	OL.propertyList[objectname.head().asSymbol()] = propnametext;
	OL.propertyList[objectposition.head().asSymbol()] = position2;
	OL.propertyList[objectscale.head().asSymbol()] = scale;
	OL.propertyList[objectrotation.head().asSymbol()] = titlerotation;

	//AL
	Expression position3(Atom("list"));
	Expression y2(Atom(-((y_min * y_scale)) + 2));
	Expression x2(Atom(x_min * x_scale));
	position3.appendExpression(x2);
	position3.appendExpression(y2);
	AL.propertyList[objectname.head().asSymbol()] = propnametext;
	AL.propertyList[objectposition.head().asSymbol()] = position3;
	AL.propertyList[objectscale.head().asSymbol()] = scale;
	AL.propertyList[objectrotation.head().asSymbol()] = titlerotation;



	//AU
	Expression position4(Atom("list"));
	Expression y3(Atom((-(y_min * y_scale)) + 2));
	Expression x3(Atom(x_max * x_scale));
	position4.appendExpression(x3);
	position4.appendExpression(y3);
	AU.propertyList[objectname.head().asSymbol()] = propnametext;
	AU.propertyList[objectposition.head().asSymbol()] = position4;
	AU.propertyList[objectscale.head().asSymbol()] = scale;
	AU.propertyList[objectrotation.head().asSymbol()] = titlerotation;




	//////////////////////////////////////////////////////////////////////////////////////
	//lab.appendExpression(title);
	output.appendExpression(OU);
	output.appendExpression(OL);
	output.appendExpression(AL);
	output.appendExpression(AU);
	
	return lab;
}
Expression Expression::make_plot_coords(Environment &env, Expression origin_head, bool y_overlap, bool x_overlap, Expression &output)
{
	Expression coords(Atom("list"));
	Expression objectname(Atom("\"object-name\""));
	Expression propnamepoint(Atom("\"point\""));
	Expression propnameline(Atom("\"line\""));
	Environment en = env;
	double x_min = min_max_list["x_min"];
	double x_max = min_max_list["x_max"];
	double y_min = min_max_list["y_min"];
	double y_max = min_max_list["y_max"];
	double x_scale = min_max_list["x_scale"];
	double y_scale = min_max_list["y_scale"];

	Expression e = origin_head;
	//bottom point coords
	Expression x(Atom(0));
	Expression bottom_y(Atom(-(y_min * y_scale)));

	//top point coords
	//x &
	Expression top_y(Atom(-(y_max* y_scale)));

	//left point coords
	Expression y(Atom(0));
	Expression left_x(Atom(x_min* x_scale));

	//right point coords
	//y &
	Expression right_x(Atom(x_max* x_scale));

	//y-axis top point
	Expression y_axis_top_point(Atom("list"));
	y_axis_top_point.appendExpression(x);
	y_axis_top_point.appendExpression(top_y);
	y_axis_top_point.propertyList[objectname.head().asSymbol()] = propnamepoint;

	//y-axis bottom point
	Expression y_axis_bottom_point(Atom("list"));
	y_axis_bottom_point.appendExpression(x);
	y_axis_bottom_point.appendExpression(bottom_y);
	y_axis_bottom_point.propertyList[objectname.head().asSymbol()] = propnamepoint;

	//x axis left point
	Expression x_axis_left_point(Atom("list"));
	x_axis_left_point.appendExpression(left_x);
	x_axis_left_point.appendExpression(y);
	x_axis_left_point.propertyList[objectname.head().asSymbol()] = propnamepoint;

	//x axis right point
	Expression x_axis_right_point(Atom("list"));
	x_axis_right_point.appendExpression(right_x);
	x_axis_right_point.appendExpression(y);
	x_axis_right_point.propertyList[objectname.head().asSymbol()] = propnamepoint;

	//y axis line
	Expression y_axis_line(Atom("list"));
	y_axis_line.appendExpression(y_axis_bottom_point);
	y_axis_line.appendExpression(y_axis_top_point);
	y_axis_line.propertyList[objectname.head().asSymbol()] = propnameline;

	//x axis line
	Expression x_axis_line(Atom("list"));
	x_axis_line.appendExpression(x_axis_left_point);
	x_axis_line.appendExpression(x_axis_right_point);
	x_axis_line.propertyList[objectname.head().asSymbol()] = propnameline;





	if (x_overlap == true)
	{
		output.appendExpression(x_axis_line);
	}
	if (y_overlap == true)
	{
		output.appendExpression(y_axis_line);
	}


	return coords;
}
Expression Expression::plot_data(Environment & env, Expression tail_0, Expression & output, bool x_overlap, int plot)
{
	//int p_size = P;
	Expression data_points(Atom("list"));
	Expression objectname(Atom("\"object-name\""));
	Expression namepoint(Atom("\"point\""));
	Expression nameline(Atom("\"line\""));
	Expression thickprop(Atom("\"thickness\""));
	Expression propname(Atom("\"size\""));
	//double p_size = P;
	Expression thickness(Atom(0));

	//double x_min = min_max_list["x_min"];
	//double x_max = min_max_list["x_max"];
	double y_min = min_max_list["y_min"];
	double y_max = min_max_list["y_max"];
	double x_scale = min_max_list["x_scale"];
	double y_scale = min_max_list["y_scale"];
	Environment e = env;
	Expression size;

	if (plot == 0)
	{
		Expression newsize(P);
		size = newsize;
	}
	else
	{
		Expression newsize(0);
		size = newsize;
	}
	for (unsigned int i = 0; i < tail_0.m_tail.size(); i++)
	{
		Expression coordinate_pair(Atom("list"));
		
		Expression temp_x;
		Expression temp_y;
		Expression temp_pair(Atom("list"));


		temp_x = tail_0.m_tail[i].m_tail[0];
		double x_coord = (temp_x.head().asNumber()*x_scale);
		temp_y = tail_0.m_tail[i].m_tail[1];
		double y_coord = -(temp_y.head().asNumber()*y_scale);

		Expression x(x_coord);//(Atom(x_coord));
		Expression y(y_coord);//(Atom(y_coord));

		coordinate_pair.appendExpression(x);
		coordinate_pair.appendExpression(y);
		coordinate_pair.propertyList[objectname.head().asSymbol()] = namepoint;
		coordinate_pair.propertyList[propname.head().asSymbol()] = size;
		output.appendExpression(coordinate_pair);
		if (plot == 0)
		{
			Expression lollipop_stick(Atom("list"));
			Expression x_temp(x_coord);
			temp_pair.appendExpression(x_temp);


			if (x_overlap)
			{
				Expression y_temp(0);
				temp_pair.appendExpression(y_temp);
			}
			else if (!x_overlap && y_coord < 0)
			{
				Expression y_temp(-(y_min*y_scale));
				temp_pair.appendExpression(y_temp);
			}
			else if (!x_overlap && y_coord > 0)
			{
				Expression y_temp(-(y_max*y_scale));
				temp_pair.appendExpression(y_temp);
			}

			temp_pair.propertyList[objectname.head().asSymbol()] = namepoint;


			lollipop_stick.appendExpression(coordinate_pair);
			lollipop_stick.appendExpression(temp_pair);
			lollipop_stick.propertyList[objectname.head().asSymbol()] = nameline;
			lollipop_stick.propertyList[thickprop.head().asSymbol()] = thickness;
			output.appendExpression(lollipop_stick);
		}
		else if (plot == 1)
		{
			if (i >= 1)
			{
				Expression connect_the_dots(Atom("list"));
				connect_the_dots.appendExpression(coordinate_pair);

				Expression coordinate_pair2(Atom("list"));

				Expression temp_x_2;
				Expression temp_y_2;
				Expression temp_pair_2(Atom("list"));


				temp_x_2 = tail_0.m_tail[i-1].m_tail[0];
				double x_coord_2 = (temp_x_2.head().asNumber()*x_scale);
				temp_y_2 = tail_0.m_tail[i-1].m_tail[1];
				double y_coord_2 = -(temp_y_2.head().asNumber()*y_scale);

				Expression x2(x_coord_2);//(Atom(x_coord));
				Expression y2(y_coord_2);//(Atom(y_coord));

				coordinate_pair2.appendExpression(x2);
				coordinate_pair2.appendExpression(y2);
				//coordinate_pair2.propertyList[objectname.head().asSymbol()] = namepoint;
				//coordinate_pair2.propertyList[propname.head().asSymbol()] = size;
				
				connect_the_dots.appendExpression(coordinate_pair2);
				connect_the_dots.propertyList[objectname.head().asSymbol()] = nameline;
				connect_the_dots.propertyList[thickprop.head().asSymbol()] = thickness;
				//std::cout << connect_the_dots;
				output.appendExpression(connect_the_dots);





			}			
		}



	}

	return output;
}
Expression Expression::do_discrete_plot(Environment & env)
{
	if (m_tail.size() != 2)
		throw SemanticError("Error: incorrect number of arguments in call to discrete-plot");
	if (m_tail[1].head().asSymbol() != "list")
		throw SemanticError("Error: second argument in call to discrete-plot not a list");

	Expression origin_head = *this;

	Expression output(Atom("list"));

	Expression tail_1;
	Expression tail_0 = m_tail[0].eval(env);
	if (m_tail[1].m_tail.size() != 0)
		tail_1 = m_tail[1].eval(env);

	get_min_max(tail_0, tail_1);

	bool y_overlap = true;
	bool x_overlap = true;
	double x_min = min_max_list["x_min"];
	double x_max = min_max_list["x_max"];
	double y_min = min_max_list["y_min"];
	double y_max = min_max_list["y_max"];
	if (x_min > 0 || x_max < 0)
		y_overlap = false;
	if (y_min > 0 || y_max < 0)
		x_overlap = false;



	plot_data(env, tail_0, output, x_overlap, 0);
	make_plot_coords(env, origin_head, y_overlap, x_overlap, output);
	make_plot_bound(env, origin_head, output);
	get_labels(tail_1, output, 0);

	//make_plot_labels(env, origin_head);
	//output.appendExpression(exp);
	//output.appendExpression(coords);
	//output.appendExpression(lab);
	//output.appendExpression(plots);



	return output;
}
Expression Expression::create_data_pairs(Expression tail_1, Environment & env)
{
	Expression data_pairs(Atom("list"));
	double xmin = tail_1.m_tail[0].head().asNumber();
	double xmax = tail_1.m_tail[1].head().asNumber();


	double inc = (xmax - xmin) / 50;


	double temp_x = xmin;

	for (int i = 0; i <= 50; i++)
	{
		Expression to_eval(m_tail[0]);
		Expression x_coord(temp_x);
		//to_eval.appendExpression(m_tail[0]);
		to_eval.appendExpression(x_coord);
		Expression y_coord = to_eval.eval(env);
		temp_x += inc;


		//double y_coord;
		Expression temp_pair(Atom("list"));
		temp_pair.appendExpression(x_coord);
		temp_pair.appendExpression(y_coord);
		data_pairs.appendExpression(temp_pair);


		//return y_coord;
		//Expression x()


	}
	//std::cout << data_pairs;
	return data_pairs;
}


Expression Expression::do_continuous_plot(Environment & env)
{
	if (m_tail.size() != 2 && m_tail.size() != 3)
		throw SemanticError("Error: incorrect number of arguments in call to continuous-plot");
	if (m_tail[1].head().asSymbol() != "list")
		throw SemanticError("Error: second argument in call to continuous-plot not a list");
	if (m_tail.size() == 3 && m_tail[2].head().asSymbol() != "list")
		throw SemanticError("Error: third argument in call to continuous-plot not a list");

	Expression origin_head = *this;

	Expression output(Atom("list"));
	Expression tail_2(Atom("empty"));
	Expression tail_1;
	Expression tail_0 = m_tail[0].eval(env);
	if (m_tail[1].m_tail.size() != 0)
		tail_1 = m_tail[1].eval(env);
	if (m_tail.size() == 3 && m_tail[2].m_tail.size() != 0)
		tail_2 = m_tail[2].eval(env);

	Expression data = create_data_pairs(tail_1, env);
	
	get_min_max(data, tail_1);

	//std::cout << data;
	//return data;
	



	//double ymax = tail_0.m_tail[0].m_tail[1].head().asNumber();
	//double ymin = tail_0.m_tail[0].m_tail[1].head().asNumber();




	//get_min_max(tail_0, tail_1, 1);

	bool y_overlap = true;
	bool x_overlap = true;
	double x_min = min_max_list["x_min"];
	double x_max = min_max_list["x_max"];
	double y_min = min_max_list["y_min"];
	double y_max = min_max_list["y_max"];
	if (x_min > 0 || x_max < 0)
		y_overlap = false;
	if (y_min > 0 || y_max < 0)
		x_overlap = false;



	plot_data(env, data, output, x_overlap, 1);
	make_plot_coords(env, origin_head, y_overlap, x_overlap, output);
	make_plot_bound(env, origin_head, output);
	get_labels(tail_2, output, 1);

	//make_plot_labels(env, origin_head);
	//output.appendExpression(exp);
	//output.appendExpression(coords);
	//output.appendExpression(lab);
	//output.appendExpression(plots);



	return output;
}

// this is a simple recursive version. the iterative version is more
// difficult with the ast data structure used (no parent pointer).
// this limits the practical depth of our AST
Expression Expression::eval(Environment & env) {
	Expression check = env.get_exp(m_head);
	Environment lambdaEnv = env;
	if (m_head.asSymbol() == "apply") {
		return doApply(env);
	}
	else if (m_tail.empty() && m_head.asSymbol() != "list" && m_head.asSymbol() != "apply") {
		return handle_lookup(m_head, env);
	}
	// handle begin special-form
	else if (m_head.isSymbol() && m_head.asSymbol() == "begin") {
		return handle_begin(env);
	}
	// handle define special-form
	else if (m_head.isSymbol() && m_head.asSymbol() == "define") {
		return handle_define(env);
	}
	else if (m_head.isSymbol() && m_head.asSymbol() == "lambda") {
		return handle_lambda(env);
	}
	else if (m_head.isSymbol() && m_head.asSymbol() == "map")
	{
		return doMap(env, lambdaEnv);

	}
	else if (m_head.isSymbol() && m_head.asSymbol() == "set-property")
	{
		return doSetProperty(env);
		//throw SemanticError("here");
	}
	else if (m_head.isSymbol() && m_head.asSymbol() == "get-property")
	{

		return doGetProperty(env);
	}
	else if (m_head.isSymbol() && m_head.asSymbol() == "discrete-plot")
	{
		return do_discrete_plot(env);
	}
	else if (m_head.isSymbol() && m_head.asSymbol() == "continuous-plot")
	{
		return do_continuous_plot(env);
	}

	// else attempt to treat as procedure
	else {
		if (env.is_known(m_head) && check.m_head.asSymbol() == "lambda")
		{
			return doLambda(lambdaEnv, env, check);
		}


		std::vector<Expression> results;
		for (Expression::IteratorType it = m_tail.begin(); it != m_tail.end(); ++it) {
			results.push_back(it->eval(env));
		}
		return apply(m_head, results, env);
	}
}


bool definesProc(const Expression & exp)
{
	if (exp.head().asSymbol() == "conj" || exp.head().asSymbol() == "arg" || exp.head().asSymbol() == "mag" ||
		exp.head().asSymbol() == "imag" || exp.head().asSymbol() == "real" || exp.head().asSymbol() == "tan" ||
		exp.head().asSymbol() == "cos" || exp.head().asSymbol() == "sin" || exp.head().asSymbol() == "ln" ||
		exp.head().asSymbol() == "^" || exp.head().asSymbol() == "sqrt" || exp.head().asSymbol() == "/" ||
		exp.head().asSymbol() == "*" || exp.head().asSymbol() == "-" || exp.head().asSymbol() == "+" ||
		exp.head().asSymbol() == "conj")
		return true;
	else
		return false;
}

std::ostream & operator<<(std::ostream & out, const Expression & exp) {


	if (exp.head().isNone())
	{
		out << "NONE";
	}
	else
	{
		if (!exp.head().isComplex() && exp.head().asSymbol() != "length") {
			out << "(";
		}
		if (exp.head().asSymbol() != "list" && exp.head().asSymbol() != "rest" &&
			exp.head().asSymbol() != "length" && exp.head().asSymbol() != "lambda") {
			out << exp.head();

			if (definesProc(exp))
				out << " ";
		}
		for (auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); ++e) {
			out << *e;
			if (e != exp.tailConstEnd() - 1)
				out << " ";
		}
		if (!exp.head().isComplex() && exp.head().asSymbol() != "length") {
			out << ")";
		}
	}
	return out;
}

bool Expression::operator==(const Expression & exp) const noexcept {

	bool result = (m_head == exp.m_head);

	result = result && (m_tail.size() == exp.m_tail.size());

	if (result) {
		for (auto lefte = m_tail.begin(), righte = exp.m_tail.begin();
			(lefte != m_tail.end()) && (righte != exp.m_tail.end());
			++lefte, ++righte) {
			result = result && (*lefte == *righte);
		}
	}

	return result;
}

Expression Expression::getProperty(std::string str)
{
	Expression hasProperty;
	auto found = propertyList.find(str);
	if (found != propertyList.cend())
	{
		hasProperty = found->second;
	}
	return hasProperty;

}

bool operator!=(const Expression & left, const Expression & right) noexcept {

	return !(left == right);
}