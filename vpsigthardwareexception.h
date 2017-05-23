#ifndef IGTHARDWAREEXCEPTION_H_INCLUDED
#define IGTHARDWAREEXCEPTION_H_INCLUDED

#include <itkMacro.h>
#include <sstream>
#include <vector>

class Exception : public itk::ExceptionObject
{

public:
	Exception(const char *file, unsigned int lineNumber=0,
		const char *desc="None", const char *loc="Unknown") :
	itk::ExceptionObject(file,lineNumber,desc,loc){}

	virtual ~Exception() throw() {}

	itkTypeMacro(ClassName, SuperClassName);

	/** \brief Adds rethrow data to this exception. */
	void AddRethrowData(const char *file, unsigned int lineNumber, const char *message)
	{
		Exception::ReThrowData data = {file, lineNumber, message};
		this->m_RethrowData.push_back(data);
	}

	/** \return Returns how often the exception was rethrown. */
	int GetNumberOfRethrows()
	{
		return (int)m_RethrowData.size();
	}

	/** @return Returns the rethrow data of the specified rethrow number. Returns empty data, if the rethrowNumber doesn't exist.
	* @param rethrowNumber The internal number of the rethrow.
	* @param file (returnvalue) This varaiable will be filled with the file of the specified rethrow.
	* @param file (returnvalue) This varaiable will be filled with the line of the specified rethrow.
	* @param file (returnvalue) This varaiable will be filled with the message of the specified rethrow.
	*/
	void GetRethrowData(int rethrowNumber, std::string &file, int &line, std::string &message)
	{
		if ((rethrowNumber >= (int)m_RethrowData.size()) || (rethrowNumber<0))
		{
			file = "";
			line = 0;
			message = "";
			return;
		}

		file = m_RethrowData.at(rethrowNumber).RethrowClassname;
		line = m_RethrowData.at(rethrowNumber).RethrowLine;
		message = m_RethrowData.at(rethrowNumber).RethrowMessage;
	}

	/** \brief Definition of the bit shift operator for this class.*/
	template <class T> inline Exception& operator<<(const T& data)
	{
		std::stringstream ss;
		ss << this->GetDescription() << data;
		this->SetDescription(ss.str());
		return *this;
	}

	/** \brief Definition of the bit shift operator for this class (for non const data).*/
	template <class T> inline Exception& operator<<(T& data)
	{
		std::stringstream ss;
		ss << this->GetDescription() << data;
		this->SetDescription(ss.str());
		return *this;
	}

	/** \brief Definition of the bit shift operator for this class (for functions).*/
	inline Exception& operator<<(std::ostream& (*func)(std::ostream&))
	{
		std::stringstream ss;
		ss << this->GetDescription() << func;
		this->SetDescription(ss.str());
		return *this;
	}
protected:

	struct ReThrowData
	{
		std::string RethrowClassname;
		unsigned int RethrowLine;
		std::string RethrowMessage;
	};

	std::vector<ReThrowData> m_RethrowData;
};
/** The exception macro is used to throw an exception
*  (i.e., usually a condition that results in program failure).
*
*  Example usage looks like:
*  vpsThrow() << "this is error info";
*/
#define vpsThrow() throw Exception(__FILE__,__LINE__,"",ITK_LOCATION)

/** The rethrow macro is used to rethrow an existing exception. The
* rethrow information (file,line of code) is then additionally stored
* in the exception. To check if an exception was rethrown you can use
* the methods GetNumberOfRethrows() and GetRethrowData().
*
* Example usage:
* try
*   {
*   //some code that throws an exception
*   }
* catch(Exception e)
*   {
*   //here we want to rethrow the exception
*   mitkmitkReThrow(e) << "Message that will be appended to the exception (optional)";
*   }
*/
#define vpsReThrow(mitkexception) \
	mitkexception.AddRethrowData(__FILE__,__LINE__,"Rethrow by mitkReThrow macro.");\
	throw mitkexception

/** The specialized exception macro is used to throw exceptions
* in cases of specialized errors. This means the second parameter must be a class which
* inherits from Exception. An object of this exception is thrown when using the macro.
* Thus, more differentiated excaptions can be thrown, when needed.
*
* Example usage:
* mitkSpecializedExceptionMacro(MySpecializedException) << "this is error info";
*/
#define vpsThrowException(classname) throw classname(__FILE__,__LINE__,"",ITK_LOCATION)

/** Class macro for MITK exception classes.
* All MITK exception classes should derive from MITK::Exception.
*/
#define vpsExceptionClassMacro(ClassName,SuperClassName) \
	ClassName(const char *file, unsigned int lineNumber, const char *desc, const char *loc) :\
	SuperClassName(file,lineNumber,desc,loc){}\
	itkTypeMacro(ClassName, SuperClassName);\
	/** \brief Definition of the bit shift operator for this class. It can be used to add messages.*/\
	template <class T> inline ClassName& operator<<(const T& data)\
{\
	std::stringstream ss;\
	ss << this->GetDescription() << data;\
	this->SetDescription(ss.str());\
	return *this;\
}\
	/** \brief Definition of the bit shift operator for this class (for non const data).*/\
	template <class T> inline ClassName& operator<<(T& data)\
{\
	std::stringstream ss;\
	ss << this->GetDescription() << data;\
	this->SetDescription(ss.str());\
	return *this;\
}\
	/** \brief Definition of the bit shift operator for this class (for functions).*/\
	inline ClassName& operator<<(std::ostream& (*func)(std::ostream&))\
{\
	std::stringstream ss;\
	ss << this->GetDescription() << func;\
	this->SetDescription(ss.str());\
	return *this;\
}\

/**Documentation
* \brief An object of this class represents an exception of the MITK-IGT module which are releated to the hardware (e.g. connection problems, etc.).
*
* \ingroup IGT
*/  
class IGTException : public Exception //perhaps this should later derive from Exception (if there is an exception handling for complete MITK
{
public:
	vpsExceptionClassMacro(IGTException,Exception);
};

class IGTHardwareException : public IGTException
{
public:
	vpsExceptionClassMacro(IGTHardwareException,IGTException);
};

#endif
