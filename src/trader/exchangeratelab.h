#pragma once

#include "exchangeratelabapi.h"

namespace trader {

	class ExchangeratelabApi;

	class Exchangeratelab
	{
	public:
		Exchangeratelab(Poco::AutoPtr<trader::App> _app);

		~Exchangeratelab();

		Poco::Dynamic::Var invoke(const std::string& httpMethod, Poco::URI& uri);

	protected:
		Exchangeratelab(const Exchangeratelab&);
		Exchangeratelab& operator = (const Exchangeratelab&);

		ExchangeratelabApi exchangeratelabApi;
	};

}