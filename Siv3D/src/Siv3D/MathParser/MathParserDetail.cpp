//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//	Copyright (c) 2025      kestrel-90r
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "MathParserDetail.hpp"
# include <Siv3D/Unicode.hpp>

namespace s3d
{
	MathParser::MathParserDetail::MathParserDetail()
	{

	}

	MathParser::MathParserDetail::~MathParserDetail()
	{

	}

	String MathParser::MathParserDetail::getErrorMessage() const
	{
		return Unicode::FromWstring(m_errorMessage);
	}

	void MathParser::MathParserDetail::setExpression(const StringView expression)
	{
		m_errorMessage.clear();

# if SIV3D_PLATFORM(ANDROID)
		m_parser.SetExpr(expression.toUTF8());
#else
		m_parser.SetExpr(expression.toWstr());
#endif
	}

	bool MathParser::MathParserDetail::setConstant(const StringView name, const double value)
	{
		m_errorMessage.clear();

		try
		{
# if SIV3D_PLATFORM(ANDROID)
			m_parser.DefineConst(name.toUTF8(), value);
#else
			m_parser.DefineConst(name.toWstr(), value);
#endif
			return true;
		}
		catch (mu::Parser::exception_type& e)
		{
# if SIV3D_PLATFORM(ANDROID)
			m_errorMessage = std::wstring(e.GetMsg().begin(), e.GetMsg().end());
#else
			m_errorMessage = e.GetMsg();
#endif
			return false;
		}
	}

	bool MathParser::MathParserDetail::setVaribale(const StringView name, double* value)
	{
		m_errorMessage.clear();

		try
		{
# if SIV3D_PLATFORM(ANDROID)
			m_parser.DefineVar(name.toUTF8(), value);
#else
			m_parser.DefineVar(name.toWstr(), value);
#endif
			return true;
		}
		catch (mu::Parser::exception_type& e)
		{
# if SIV3D_PLATFORM(ANDROID)
			m_errorMessage = std::wstring(e.GetMsg().begin(), e.GetMsg().end());
#else
			m_errorMessage = e.GetMsg();
#endif
			return false;
		}
	}

	bool MathParser::MathParserDetail::setPrefixOperator(const StringView name, Fty1 f)
	{
		m_errorMessage.clear();

		try
		{
# if SIV3D_PLATFORM(ANDROID)
			m_parser.DefineInfixOprt(name.toUTF8(), f);
#else
			m_parser.DefineInfixOprt(name.toWstr(), f);
#endif
			return true;
		}
		catch (mu::Parser::exception_type& e)
		{
# if SIV3D_PLATFORM(ANDROID)
			m_errorMessage = std::wstring(e.GetMsg().begin(), e.GetMsg().end());
#else
			m_errorMessage = e.GetMsg();
#endif
			return false;
		}
	}

	bool MathParser::MathParserDetail::setPostfixOperator(const StringView name, Fty1 f)
	{
		m_errorMessage.clear();

		try
		{
# if SIV3D_PLATFORM(ANDROID)
			m_parser.DefinePostfixOprt(name.toUTF8(), f);
#else
			m_parser.DefinePostfixOprt(name.toWstr(), f);
#endif
			return true;
		}
		catch (mu::Parser::exception_type& e)
		{
# if SIV3D_PLATFORM(ANDROID)
			m_errorMessage = std::wstring(e.GetMsg().begin(), e.GetMsg().end());
#else
			m_errorMessage = e.GetMsg();
#endif
			return false;
		}
	}

	void MathParser::MathParserDetail::removeVariable(const StringView name)
	{
		m_errorMessage.clear();

# if SIV3D_PLATFORM(ANDROID)
		m_parser.RemoveVar(name.toUTF8());
#else
		m_parser.RemoveVar(name.toWstr());
#endif
	}

	void MathParser::MathParserDetail::clear()
	{
		m_errorMessage.clear();

# if SIV3D_PLATFORM(ANDROID)
		m_parser.SetExpr(mu::string_type{});
#else
		m_parser.SetExpr(mu::string_type{});
#endif
		m_parser.ClearConst();
		m_parser.ClearVar();
		m_parser.ClearFun();
		m_parser.ClearInfixOprt();
		m_parser.ClearPostfixOprt();
		m_parser.ClearOprt();
	}

	String MathParser::MathParserDetail::getExpression() const
	{
# if SIV3D_PLATFORM(ANDROID)
		return Unicode::FromUTF8(m_parser.GetExpr());
#else
		return Unicode::FromWstring(m_parser.GetExpr());
#endif
	}

	HashTable<String, double*> MathParser::MathParserDetail::getUsedVariables() const
	{
		m_errorMessage.clear();

		try
		{
			HashTable<String, double*> result;

			for (const auto& pair : m_parser.GetUsedVar())
			{
# if SIV3D_PLATFORM(ANDROID)
				result.emplace(Unicode::FromUTF8(pair.first), pair.second);
#else
				result.emplace(Unicode::FromWstring(pair.first), pair.second);
#endif
			}

			return result;
		}
		catch (mu::Parser::exception_type& e)
		{
# if SIV3D_PLATFORM(ANDROID)
			m_errorMessage = std::wstring(e.GetMsg().begin(), e.GetMsg().end());
#else
			m_errorMessage = e.GetMsg();
#endif
			return{};
		}
	}

	HashTable<String, double*> MathParser::MathParserDetail::getVariables() const
	{
		m_errorMessage.clear();

		try
		{
			HashTable<String, double*> result;

			for (const auto& pair : m_parser.GetVar())
			{
# if SIV3D_PLATFORM(ANDROID)
				result.emplace(Unicode::FromUTF8(pair.first), pair.second);
#else
				result.emplace(Unicode::FromWstring(pair.first), pair.second);
#endif
			}

			return result;
		}
		catch (mu::Parser::exception_type& e)
		{
# if SIV3D_PLATFORM(ANDROID)
			m_errorMessage = std::wstring(e.GetMsg().begin(), e.GetMsg().end());
#else
			m_errorMessage = e.GetMsg();
#endif
			return{};
		}
	}

	HashTable<String, double> MathParser::MathParserDetail::getConstants() const
	{
		m_errorMessage.clear();

		try
		{
			HashTable<String, double> result;

			for (const auto& pair : m_parser.GetConst())
			{
# if SIV3D_PLATFORM(ANDROID)
				result.emplace(Unicode::FromUTF8(pair.first), pair.second);
#else
				result.emplace(Unicode::FromWstring(pair.first), pair.second);
#endif
			}

			return result;
		}
		catch (mu::Parser::exception_type& e)
		{
# if SIV3D_PLATFORM(ANDROID)
			m_errorMessage = std::wstring(e.GetMsg().begin(), e.GetMsg().end());
#else
			m_errorMessage = e.GetMsg();
#endif
			return{};
		}
	}

	String MathParser::MathParserDetail::validNameCharacters() const
	{
# if SIV3D_PLATFORM(ANDROID)
		return Unicode::FromUTF8(m_parser.ValidNameChars());
#else
		return Unicode::FromWstring(m_parser.ValidNameChars());
#endif
	}

	String MathParser::MathParserDetail::validPrefixCharacters() const
	{
# if SIV3D_PLATFORM(ANDROID)
		return Unicode::FromUTF8(m_parser.ValidInfixOprtChars());
#else
		return Unicode::FromWstring(m_parser.ValidInfixOprtChars());
#endif
	}

	String MathParser::MathParserDetail::validPostfixCharacters() const
	{
# if SIV3D_PLATFORM(ANDROID)
		return Unicode::FromUTF8(m_parser.ValidOprtChars());
#else
		return Unicode::FromWstring(m_parser.ValidOprtChars());
#endif
	}

	Optional<double> MathParser::MathParserDetail::evalOpt() const
	{
		m_errorMessage.clear();

		try
		{
			return m_parser.Eval();
		}
		catch (mu::Parser::exception_type& e)
		{
# if SIV3D_PLATFORM(ANDROID)
			m_errorMessage = std::wstring(e.GetMsg().begin(), e.GetMsg().end());
#else
			m_errorMessage = e.GetMsg();
#endif
			return none;
		}
	}

	Array<double> MathParser::MathParserDetail::evalArray() const
	{
		m_errorMessage.clear();

		Array<double> result;

		try
		{
			int32 num_results;
			const double* v = m_parser.Eval(num_results);
			result.assign(v, v + num_results);
		}
		catch (mu::Parser::exception_type& e)
		{
# if SIV3D_PLATFORM(ANDROID)
			m_errorMessage = std::wstring(e.GetMsg().begin(), e.GetMsg().end());
#else
			m_errorMessage = e.GetMsg();
#endif
		}

		return result;
	}

	void MathParser::MathParserDetail::eval(double* dst, const size_t count) const
	{
		m_errorMessage.clear();

		try
		{
			int32 num_results;

			const double* v = m_parser.Eval(num_results);

			size_t i = 0;

			for (; i < static_cast<size_t>(num_results); ++i)
			{
				dst[i] = v[i];
			}

			for (; i < count; ++i)
			{
				dst[i] = Math::NaN;
			}
		}
		catch (mu::Parser::exception_type& e)
		{
# if SIV3D_PLATFORM(ANDROID)
			m_errorMessage = std::wstring(e.GetMsg().begin(), e.GetMsg().end());
#else
			m_errorMessage = e.GetMsg();
#endif
			for (size_t i = 0; i < count; ++i)
			{
				dst[i] = Math::NaN;
			}
		}
	}
}
