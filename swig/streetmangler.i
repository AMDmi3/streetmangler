%module streetmangler
%include "std_string.i"
%include "std_vector.i"
%include "typemaps.i"

class StatusPart {
public:
        StatusPart(int priority, const std::string& full, const std::string& canonical, const std::string& abbrev);
        const std::string& GetFull() const;
        const std::string& GetCanonical() const;
        const std::string& GetAbbrev() const;

        bool IsPrior(const StatusPart* other) const;
};

%nestedworkaround Locale::StatusPart;

%{
#include "streetmangler/name.hh"
#include "streetmangler/locale.hh"
#include "streetmangler/database.hh"

using StreetMangler::Name;
using StreetMangler::Locale;
using StreetMangler::Database;
%}

%{
typedef Locale::StatusPart StatusPart;
%}

class Locale {
public:
        Locale(const std::string& name);

        const StatusPart* FindStatus(const std::string& name) const;
};

%rename(_Name) Name;
class Name {
public:
        Name(const std::string& name, const Locale& locale);

        std::string Join(int flags = 0);
};

namespace std {
        %template(StringVector) vector<string>;
};

%rename(_Database) Database;

class Database {
public:
        Database(const Locale& locale);
        virtual ~Database();
        void Load(const char* filename);
        void Add(const std::string& name);

        int CheckExactMatch(const std::string& name) const;
        int CheckExactMatch(Name& name) const;
};

%extend Database {
        std::vector<std::string> CheckCanonicalForm(const std::string &name) {
                std::vector<std::string> v;
                self->CheckCanonicalForm(name, v);
                return v;
        }

        std::vector<std::string> CheckSpelling(const std::string &name, int depth = 1) {
                std::vector<std::string> v;
                self->CheckSpelling(name, v, depth);
                return v;
        }

        std::vector<std::string> CheckStrippedStatus(const std::string &name) {
                std::vector<std::string> v;
                self->CheckStrippedStatus(name, v);
                return v;
        }

        std::vector<std::string> CheckCanonicalForm(Name &name) {
                std::vector<std::string> v;
                self->CheckCanonicalForm(name, v);
                return v;
        }

        std::vector<std::string> CheckSpelling(Name &name, int depth = 1) {
                std::vector<std::string> v;
                self->CheckSpelling(name, v, depth);
                return v;
        }

        std::vector<std::string> CheckStrippedStatus(Name &name) {
                std::vector<std::string> v;
                self->CheckStrippedStatus(name, v);
                return v;
        }
};

%pythoncode %{
def _unicode_args(f):
    def _f(self, s, *a, **kw):
        if isinstance(s, unicode):
            s = s.encode('utf-8')
        return f(self, s, *a, **kw)
    _f.__name__ = f.__name__
    _f.__doc__ = f.__doc__
    return _f

def _unicode_ret(f):
    def _f(*a, **kw):
        return map(lambda x: unicode(x, 'utf-8'), f(*a, **kw))
    _f.__name__ = f.__name__
    _f.__doc__ = f.__doc__
    return _f

_unicode_args_ret = lambda f: _unicode_ret(_unicode_args(f))

class Database(_Database):
    def __init__(self, locale):
        _Database.__init__(self, locale)
        self._locale = locale

    def GetLocale(self):
        return self._locale

    Add = _unicode_args(_Database.Add)
    CheckExactMatch = _unicode_args(_Database.CheckExactMatch)
    CheckCanonicalForm = _unicode_args_ret(_Database.CheckCanonicalForm)
    CheckSpelling = _unicode_args_ret(_Database.CheckSpelling)
    CheckStrippedStatus = _unicode_args_ret(_Database.CheckStrippedStatus)

class Name(_Name):
    __init__ = _unicode_args(_Name.__init__)

    def Join(self, *a, **kw):
        return _Name.Join(self, *a, **kw).decode('utf-8')

    def __unicode__(self):
        return self.Join()
%}
