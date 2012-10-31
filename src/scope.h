#ifndef SCOPE_H_
#define SCOPE_H_

#include <boost/function.hpp>
#include <boost/bind.hpp>

struct is_a_scope
{
    virtual long getIdx() = 0;

protected:
    static long next()
    {
        static long counter = 0;
        return (++counter);
    }
    is_a_scope() {}
    virtual ~is_a_scope() {}
    is_a_scope( const is_a_scope& );
    const is_a_scope& operator=( const is_a_scope& );
};

typedef is_a_scope* is_a_scope_ptr;

struct NOSCOPE : public is_a_scope
{
    template<typename TYPE> static boost::function<boost::shared_ptr<TYPE>()> scope(boost::function<boost::shared_ptr<TYPE>()> unscopedProvider)
    {
        std::cout << "NOSCOPE::scope called" << std::endl;
        return unscopedProvider;
    }

    static NOSCOPE* instance()
    {
    	static NOSCOPE* m_instance = new NOSCOPE();
        return m_instance;
    }

    static std::string name()
    {
        return "NOSCOPE";
    }

    static long const idx()
    {
        static long counter = is_a_scope::next();
        return counter;
    }

    long getIdx()
    {
        return idx();
    }
private:
    NOSCOPE() {}
    ~NOSCOPE() {};
    NOSCOPE( const NOSCOPE& );
    const NOSCOPE& operator=( const NOSCOPE& );
};


struct LoggerScope : public is_a_scope
{
    static LoggerScope* instance()
    {
    	static LoggerScope* m_instance = new LoggerScope();
        return m_instance;
    }

    static std::string name()
    {
        return "LoggerScope";
    }

    static long const idx()
    {
        static long counter = is_a_scope::next();
        return counter;
    }

    long getIdx()
    {
        return idx();
    }

    template<typename TYPE> static boost::function<boost::shared_ptr<TYPE>()> scope(boost::function<boost::shared_ptr<TYPE>()> unscopedProvider);

private:

    LoggerScope() {}
    ~LoggerScope() {};
    LoggerScope( const LoggerScope& );
    const LoggerScope& operator=( const LoggerScope& );

    template<typename TYPE> static boost::shared_ptr<TYPE> scopedProvider(boost::function<boost::shared_ptr<TYPE>()> unscopedProvider)
    {
        std::cout << "LoggerScope scoping called" << std::endl;
        boost::shared_ptr<TYPE> res = unscopedProvider();
        std::cout << "LoggerScope scoping finished" << std::endl;
        return res;
    }
};

template<typename TYPE> boost::function<boost::shared_ptr<TYPE>()> LoggerScope::scope(boost::function<boost::shared_ptr<TYPE>()> unscopedProvider)
{
    std::cout << "LoggerScope::scope called" << std::endl;

    return boost::bind(LoggerScope::scopedProvider<TYPE>, unscopedProvider);
}

struct Scope
{
	static is_a_scope_ptr getCurrentScope()
	{
		return *getCurrentScopePtr();
	}


    template<typename SCOPE> static void enter()
    {
    	is_a_scope_ptr* val = getCurrentScopePtr();
    	*val = SCOPE::instance();
    }

    static void exit()
    {
    	is_a_scope_ptr* val = getCurrentScopePtr();
    	*val = NULL;
    }

private:
	static is_a_scope_ptr* getCurrentScopePtr()
	{
		static is_a_scope_ptr currentScope = NULL;

		return &currentScope;
	}
};

#endif /* SCOPE_H_ */
