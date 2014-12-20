using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Globalization;



using System;



public class Parser {
	public const int _EOF = 0;
	public const int _ident = 1;
	public const int _number = 2;
	public const int _hexnumber = 3;
	public const int _priority = 4;
	public const int maxT = 48;

	const bool T = true;
	const bool x = false;
	const int minErrDist = 2;
	
	public Scanner scanner;
	public Errors  errors;

	public Token t;    // last recognized token
	public Token la;   // lookahead token
	int errDist = minErrDist;

public model2mbed.Declarations decls = new model2mbed.Declarations();

bool IsChanPriority()
{
	scanner.ResetPeek();
	var x = scanner.Peek();
	if(x.kind == _priority)
		return true;
	return false;
}



	public Parser(Scanner scanner) {
		this.scanner = scanner;
		errors = new Errors();
	}

	void SynErr (int n) {
		if (errDist >= minErrDist) errors.SynErr(la.line, la.col, n);
		errDist = 0;
	}

	public void SemErr (string msg) {
		if (errDist >= minErrDist) errors.SemErr(t.line, t.col, msg);
		errDist = 0;
	}
	
	void Get () {
		for (;;) {
			t = la;
			la = scanner.Scan();
			if (la.kind <= maxT) { ++errDist; break; }

			la = t;
		}
	}
	
	void Expect (int n) {
		if (la.kind==n) Get(); else { SynErr(n); }
	}
	
	bool StartOf (int s) {
		return set[s, la.kind];
	}
	
	void ExpectWeak (int n, int follow) {
		if (la.kind == n) Get();
		else {
			SynErr(n);
			while (!StartOf(follow)) Get();
		}
	}


	bool WeakSeparator(int n, int syFol, int repFol) {
		int kind = la.kind;
		if (kind == n) {Get(); return true;}
		else if (StartOf(repFol)) {return false;}
		else {
			SynErr(n);
			while (!(set[syFol, kind] || set[repFol, kind] || set[0, kind])) {
				Get();
				kind = la.kind;
			}
			return StartOf(syFol);
		}
	}

	
	void UPPAALDECL() {
		while (StartOf(1)) {
			Declaration();
		}
	}

	void Declaration() {
		if (la.kind == 5) {
			Get();
			Typedef();
		} else if (IsChanPriority()) {
			Expect(6);
			Expect(4);
			ChanPriorityDecl();
		} else if (StartOf(2)) {
			VarDecl();
		} else SynErr(49);
		Expect(7);
	}

	void Typedef() {
		string name; model2mbed.TypeDecl typdecl; 
		TypeDecl(out typdecl);
		Ident(out name);
		decls.Add(new model2mbed.TypedefDecl() { Name = name, Type = typdecl } ); 
	}

	void ChanPriorityDecl() {
		model2mbed.Expression expr = null; 
		if (StartOf(3)) {
			ChanExpr(out expr);
			decls.setChannelPriority(expr); 
		} else if (la.kind == 47) {
			Get();
			decls.setChannelDefaultPriority(); 
		} else SynErr(50);
		while (la.kind == 8 || la.kind == 33) {
			if (la.kind == 8) {
				Get();
			} else {
				Get();
				decls.nextChannelPriority(); 
			}
			if (StartOf(3)) {
				ChanExpr(out expr);
				decls.setChannelPriority(expr); 
			} else if (la.kind == 47) {
				Get();
				decls.setChannelDefaultPriority(); 
			} else SynErr(51);
		}
	}

	void VarDecl() {
		model2mbed.TypeDecl type;  
		TypeDecl(out type);
		VarNameDecl(type);
		while (la.kind == 8) {
			Get();
			VarNameDecl(type);
		}
	}

	void TypeDecl(out model2mbed.TypeDecl data) {
		data = new model2mbed.TypeDecl(); string tname; model2mbed.Expression arg1 = null, arg2 = null; 
		while (StartOf(4)) {
			if (la.kind == 12) {
				Get();
				data.Const = true; 
			} else if (la.kind == 13) {
				Get();
				data.Urgent = true; 
			} else if (la.kind == 14) {
				Get();
				data.Broadcast = true; 
			} else {
				Get();
			}
		}
		if (la.kind == 16) {
			Get();
			data.Type = model2mbed.VarType.Int; 
		} else if (la.kind == 17) {
			Get();
			data.Type = model2mbed.VarType.Int; 
		} else if (la.kind == 18) {
			Get();
			data.Type = model2mbed.VarType.Clock; 
		} else if (la.kind == 6) {
			Get();
			data.Type = model2mbed.VarType.Channel; 
		} else if (la.kind == 1) {
			Ident(out tname);
			data = decls.normalizeType(data, tname); 
		} else SynErr(52);
		if (la.kind == 9) {
			Get();
			Expression(out arg1);
			Expect(8);
			Expression(out arg2);
			Expect(10);
			int low, high;
			if(!decls.getExprValue(arg1, out low) || !decls.getExprValue(arg2, out high))
			throw new model2mbed.ParseException("Array ranges must evaluate to consts!");
			data.SetRange(low, high); 
			
		}
	}

	void Ident(out string value) {
		Expect(1);
		value = t.val; 
	}

	void VarNameDecl(model2mbed.TypeDecl type) {
		string name; model2mbed.Expression arg1 = null, arg2 = null; 
		Ident(out name);
		model2mbed.VarDecl vardecl = new model2mbed.VarDecl(name, type); decls.Add(vardecl); 
		if (la.kind == 9) {
			Get();
			Expression(out arg1);
			Expect(10);
			int low = 0, length = 0;
			if(!decls.getArrSizeValue(arg1, out low, out length))
			throw new FatalError("Could not calculate array size!");
			vardecl.IsArray = true;
			vardecl.ArrLow = low; 
			vardecl.ArrLength = length; 
			
		}
		if (la.kind == 11) {
			Get();
			Expression(out arg2);
			vardecl.Expr = arg2; vardecl.HasExpr = true; 
		}
	}

	void Expression(out model2mbed.Expression expr) {
		model2mbed.Expression arg1 = null; 
		LogWordOrExpr(out arg1);
		expr = arg1; 
	}

	void LogWordOrExpr(out model2mbed.Expression expr) {
		model2mbed.Expression arg1 = null, arg2 = null; 
		LogWordAndExpr(out arg1);
		expr = arg1; 
		if (la.kind == 19) {
			Get();
			LogWordAndExpr(out arg2);
			expr = new model2mbed.Expression(model2mbed.Expression.Funcs.LogicalOr, arg1, arg2);  
		}
	}

	void LogWordAndExpr(out model2mbed.Expression expr) {
		model2mbed.Expression arg1 = null, arg2 = null; 
		LogWordNotExpr(out arg1);
		expr = arg1; 
		if (la.kind == 20) {
			Get();
			LogWordNotExpr(out arg2);
			expr = new model2mbed.Expression(model2mbed.Expression.Funcs.LogicalAnd, arg1, arg2); 
		}
	}

	void LogWordNotExpr(out model2mbed.Expression expr) {
		model2mbed.Expression arg1 = null; expr = null; 
		if (la.kind == 21) {
			Get();
			CondExpr(out arg1);
			expr = new model2mbed.Expression(model2mbed.Expression.Funcs.LogicalNot, arg1); 
		} else if (StartOf(5)) {
			CondExpr(out arg1);
			expr = arg1; 
		} else SynErr(53);
	}

	void CondExpr(out model2mbed.Expression expr) {
		model2mbed.Expression arg1 = null, arg2 = null, arg3 = null; 
		LogOrExpr(out arg1);
		expr = arg1; 
		if (la.kind == 22) {
			Get();
			CondExpr(out arg2);
			Expect(23);
			CondExpr(out arg3);
			expr = new model2mbed.Expression(model2mbed.Expression.Funcs.IfThenElse, arg1, arg2, arg3); 
		}
	}

	void LogOrExpr(out model2mbed.Expression expr) {
		model2mbed.Expression arg1 = null, arg2 = null; 
		LogAndExpr(out arg1);
		expr = arg1; 
		if (la.kind == 24) {
			Get();
			LogAndExpr(out arg2);
			expr = new model2mbed.Expression(model2mbed.Expression.Funcs.LogicalOr, arg1, arg2); 
		}
	}

	void LogAndExpr(out model2mbed.Expression expr) {
		model2mbed.Expression arg1 = null, arg2 = null; 
		BitExpr(out arg1);
		expr = arg1; 
		if (la.kind == 25) {
			Get();
			BitExpr(out arg2);
			expr = new model2mbed.Expression(model2mbed.Expression.Funcs.LogicalAnd, arg1, arg2); 
		}
	}

	void BitExpr(out model2mbed.Expression expr) {
		model2mbed.Expression arg1 = null, arg2 = null; 
		EqlExpr(out arg1);
		expr = arg1; 
		if (la.kind == 26 || la.kind == 27 || la.kind == 28) {
			if (la.kind == 26) {
				Get();
				EqlExpr(out arg2);
				expr = new model2mbed.Expression(model2mbed.Expression.Funcs.BitAnd, arg1, arg2); 
			} else if (la.kind == 27) {
				Get();
				EqlExpr(out arg2);
				expr = new model2mbed.Expression(model2mbed.Expression.Funcs.BitXor, arg1, arg2); 
			} else {
				Get();
				EqlExpr(out arg2);
				expr = new model2mbed.Expression(model2mbed.Expression.Funcs.BitOr, arg1, arg2); 
			}
		}
	}

	void EqlExpr(out model2mbed.Expression expr) {
		model2mbed.Expression arg1 = null, arg2 = null; 
		RelExpr(out arg1);
		expr = arg1; 
		if (la.kind == 29 || la.kind == 30) {
			if (la.kind == 29) {
				Get();
				RelExpr(out arg2);
				expr = new model2mbed.Expression(model2mbed.Expression.Funcs.Equal, arg1, arg2); 
			} else {
				Get();
				RelExpr(out arg2);
				expr = new model2mbed.Expression(model2mbed.Expression.Funcs.NotEqual, arg1, arg2); 
			}
		}
	}

	void RelExpr(out model2mbed.Expression expr) {
		model2mbed.Expression arg1 = null, arg2 = null; 
		MinMaxExpr(out arg1);
		expr = arg1; 
		if (StartOf(6)) {
			if (la.kind == 31) {
				Get();
				MinMaxExpr(out arg2);
				expr = new model2mbed.Expression(model2mbed.Expression.Funcs.Larger, arg1, arg2); 
			} else if (la.kind == 32) {
				Get();
				MinMaxExpr(out arg2);
				expr = new model2mbed.Expression(model2mbed.Expression.Funcs.LargerEqual, arg1, arg2); 
			} else if (la.kind == 33) {
				Get();
				MinMaxExpr(out arg2);
				expr = new model2mbed.Expression(model2mbed.Expression.Funcs.Smaller, arg1, arg2); 
			} else {
				Get();
				MinMaxExpr(out arg2);
				expr = new model2mbed.Expression(model2mbed.Expression.Funcs.SmallEqual, arg1, arg2); 
			}
		}
	}

	void MinMaxExpr(out model2mbed.Expression expr) {
		model2mbed.Expression arg1 = null, arg2 = null; 
		AddExpr(out arg1);
		expr = arg1; 
		if (la.kind == 35 || la.kind == 36) {
			if (la.kind == 35) {
				Get();
				AddExpr(out arg2);
				expr = new model2mbed.Expression(model2mbed.Expression.Funcs.Min, arg1, arg2); 
			} else {
				Get();
				AddExpr(out arg2);
				expr = new model2mbed.Expression(model2mbed.Expression.Funcs.Max, arg1, arg2); 
			}
		}
	}

	void AddExpr(out model2mbed.Expression expr) {
		model2mbed.Expression arg1 = null, arg2 = null; 
		MultExpr(out arg1);
		expr = arg1; 
		if (la.kind == 37 || la.kind == 38) {
			if (la.kind == 37) {
				Get();
				MultExpr(out arg2);
				expr = new model2mbed.Expression(model2mbed.Expression.Funcs.BinPlus, arg1, arg2); 
			} else {
				Get();
				MultExpr(out arg2);
				expr = new model2mbed.Expression(model2mbed.Expression.Funcs.BinMinus, arg1, arg2); 
			}
		}
	}

	void MultExpr(out model2mbed.Expression expr) {
		model2mbed.Expression arg1 = null, arg2 = null; 
		UnaryExpr(out arg1);
		expr = arg1; 
		if (la.kind == 39 || la.kind == 40 || la.kind == 41) {
			if (la.kind == 39) {
				Get();
				UnaryExpr(out arg2);
				expr = new model2mbed.Expression(model2mbed.Expression.Funcs.Multi, arg1, arg2); 
			} else if (la.kind == 40) {
				Get();
				UnaryExpr(out arg2);
				expr = new model2mbed.Expression(model2mbed.Expression.Funcs.Divide, arg1, arg2); 
			} else {
				Get();
				UnaryExpr(out arg2);
				expr = new model2mbed.Expression(model2mbed.Expression.Funcs.Modulus, arg1, arg2); 
			}
		}
	}

	void UnaryExpr(out model2mbed.Expression expr) {
		model2mbed.Expression arg1 = null; expr = null; 
		switch (la.kind) {
		case 1: case 2: case 3: case 45: {
			PostfixExpr(out arg1);
			expr = arg1; 
			break;
		}
		case 37: {
			Get();
			PostfixExpr(out arg1);
			expr = new model2mbed.Expression(model2mbed.Expression.Funcs.UniPlus, arg1, null); 
			break;
		}
		case 38: {
			Get();
			PostfixExpr(out arg1);
			expr = new model2mbed.Expression(model2mbed.Expression.Funcs.UniMinus, arg1, null); 
			break;
		}
		case 42: {
			Get();
			PostfixExpr(out arg1);
			expr = new model2mbed.Expression(model2mbed.Expression.Funcs.UniPlusPlusPre, arg1, null); 
			break;
		}
		case 43: {
			Get();
			PostfixExpr(out arg1);
			expr = new model2mbed.Expression(model2mbed.Expression.Funcs.UniMinusMinusPre, arg1, null); 
			break;
		}
		case 44: {
			Get();
			UnaryExpr(out arg1);
			expr = new model2mbed.Expression(model2mbed.Expression.Funcs.LogicalNot, arg1, null); 
			break;
		}
		default: SynErr(54); break;
		}
	}

	void PostfixExpr(out model2mbed.Expression expr) {
		model2mbed.Expression arg1 = null, arg2 = null; 
		Primary(out arg1);
		expr = arg1; 
		if (StartOf(7)) {
			if (la.kind == 9) {
				Get();
				Expression(out arg2);
				Expect(10);
				expr = new model2mbed.Expression(model2mbed.Expression.Funcs.ArrayIndex, arg1, arg2); 
			} else if (la.kind == 45) {
				Get();
				List<model2mbed.Expression> funcArgs = new List<model2mbed.Expression>(); 
				if (StartOf(8)) {
					Expression(out arg2);
					funcArgs.Add(arg2); 
					while (la.kind == 8) {
						Get();
						Expression(out arg2);
					}
					funcArgs.Add(arg2); 
				}
				Expect(46);
				expr = model2mbed.Expression.CreateCustomFunc(arg1, funcArgs); 
			} else if (la.kind == 42) {
				Get();
				expr = new model2mbed.Expression(model2mbed.Expression.Funcs.UniPlusPlusPost, arg1, null); 
			} else {
				Get();
				expr = new model2mbed.Expression(model2mbed.Expression.Funcs.UniMinusMinusPost, arg1, null); 
			}
		}
	}

	void Primary(out model2mbed.Expression expr) {
		model2mbed.Expression arg1 = null; expr = null; 
		if (la.kind == 1) {
			Get();
			expr = new model2mbed.Expression(t.val); 
		} else if (la.kind == 2) {
			Get();
			expr = new model2mbed.Expression(Int32.Parse(t.val)); 
		} else if (la.kind == 3) {
			Get();
			expr = new model2mbed.Expression(Int32.Parse(t.val, NumberStyles.HexNumber)); 
		} else if (la.kind == 45) {
			Get();
			Expression(out arg1);
			Expect(46);
			expr = arg1; 
		} else SynErr(55);
	}

	void ChanExpr(out model2mbed.Expression expr) {
		model2mbed.Expression arg1 = null, arg2 = null; 
		Primary(out arg1);
		expr = arg1; 
		if (la.kind == 9) {
			Get();
			Expression(out arg2);
			Expect(10);
			expr = new model2mbed.Expression(model2mbed.Expression.Funcs.ArrayIndex, arg1, arg2); 
		}
	}



	public void Parse() {
		la = new Token();
		la.val = "";		
		Get();
		UPPAALDECL();
		Expect(0);

	}
	
	static readonly bool[,] set = {
		{T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,T,x,x, x,T,T,x, x,x,x,x, T,T,T,T, T,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,T,x,x, x,x,T,x, x,x,x,x, T,T,T,T, T,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, T,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,x, x,x,T,T, T,T,x,x, x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, x,T,x,x, x,x},
		{x,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,x, x,x,T,T, T,T,x,x, x,x}

	};
} // end Parser


public class Errors {
	public int count = 0;                                    // number of errors detected
	public System.IO.TextWriter errorStream = Console.Out;   // error messages go to this stream
	public string errMsgFormat = "-- line {0} col {1}: {2}"; // 0=line, 1=column, 2=text

	public virtual void SynErr (int line, int col, int n) {
		string s;
		switch (n) {
			case 0: s = "EOF expected"; break;
			case 1: s = "ident expected"; break;
			case 2: s = "number expected"; break;
			case 3: s = "hexnumber expected"; break;
			case 4: s = "priority expected"; break;
			case 5: s = "\"typedef\" expected"; break;
			case 6: s = "\"chan\" expected"; break;
			case 7: s = "\";\" expected"; break;
			case 8: s = "\",\" expected"; break;
			case 9: s = "\"[\" expected"; break;
			case 10: s = "\"]\" expected"; break;
			case 11: s = "\"=\" expected"; break;
			case 12: s = "\"const\" expected"; break;
			case 13: s = "\"urgent\" expected"; break;
			case 14: s = "\"broadcast\" expected"; break;
			case 15: s = "\"meta\" expected"; break;
			case 16: s = "\"int\" expected"; break;
			case 17: s = "\"bool\" expected"; break;
			case 18: s = "\"clock\" expected"; break;
			case 19: s = "\"or\" expected"; break;
			case 20: s = "\"and\" expected"; break;
			case 21: s = "\"not\" expected"; break;
			case 22: s = "\"?\" expected"; break;
			case 23: s = "\":\" expected"; break;
			case 24: s = "\"||\" expected"; break;
			case 25: s = "\"&&\" expected"; break;
			case 26: s = "\"&\" expected"; break;
			case 27: s = "\"^\" expected"; break;
			case 28: s = "\"|\" expected"; break;
			case 29: s = "\"==\" expected"; break;
			case 30: s = "\"!=\" expected"; break;
			case 31: s = "\">\" expected"; break;
			case 32: s = "\">=\" expected"; break;
			case 33: s = "\"<\" expected"; break;
			case 34: s = "\"<=\" expected"; break;
			case 35: s = "\"<?\" expected"; break;
			case 36: s = "\">?\" expected"; break;
			case 37: s = "\"+\" expected"; break;
			case 38: s = "\"-\" expected"; break;
			case 39: s = "\"*\" expected"; break;
			case 40: s = "\"/\" expected"; break;
			case 41: s = "\"%\" expected"; break;
			case 42: s = "\"++\" expected"; break;
			case 43: s = "\"--\" expected"; break;
			case 44: s = "\"!\" expected"; break;
			case 45: s = "\"(\" expected"; break;
			case 46: s = "\")\" expected"; break;
			case 47: s = "\"default\" expected"; break;
			case 48: s = "??? expected"; break;
			case 49: s = "invalid Declaration"; break;
			case 50: s = "invalid ChanPriorityDecl"; break;
			case 51: s = "invalid ChanPriorityDecl"; break;
			case 52: s = "invalid TypeDecl"; break;
			case 53: s = "invalid LogWordNotExpr"; break;
			case 54: s = "invalid UnaryExpr"; break;
			case 55: s = "invalid Primary"; break;

			default: s = "error " + n; break;
		}
		errorStream.WriteLine(errMsgFormat, line, col, s);
		count++;
	}

	public virtual void SemErr (int line, int col, string s) {
		errorStream.WriteLine(errMsgFormat, line, col, s);
		count++;
	}
	
	public virtual void SemErr (string s) {
		errorStream.WriteLine(s);
		count++;
	}
	
	public virtual void Warning (int line, int col, string s) {
		errorStream.WriteLine(errMsgFormat, line, col, s);
	}
	
	public virtual void Warning(string s) {
		errorStream.WriteLine(s);
	}
} // Errors


public class FatalError: Exception {
	public FatalError(string m): base(m) {}
}
