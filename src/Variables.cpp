#include "Variables.h"
#include "LinearCstr.h"

void Variable::set_id(int i)
{
    id = i;
}

int Variable::get_id() const
{
    return id;
}

void Variable::set_name(const std::string &n)
{
    name = n;
}
const std::string &Variable::get_name() const
{
    return name;
}

LinearCstr Variable::operator=(int i) const
{
	return LinearCstr(1*(*this), LinearCstr::EQUAL, i);
}

std::ostream &operator<<(std::ostream &os, const Variable &v)
{
	os << v.get_name();

	return os;
}

