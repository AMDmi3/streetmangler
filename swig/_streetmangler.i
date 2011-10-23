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
using namespace StreetMangler;
%}

%{
typedef Locale::StatusPart StatusPart;
%}

class Locale {
public:
        Locale(const std::string& name);

        const StatusPart* FindStatus(const std::string& name) const;
};

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

        std::vector<std::string> CheckSpelling(const std::string &name) {
                std::vector<std::string> v;
                self->CheckSpelling(name, v);
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

        std::vector<std::string> CheckSpelling(Name &name) {
                std::vector<std::string> v;
                self->CheckSpelling(name, v);
                return v;
        }

        std::vector<std::string> CheckStrippedStatus(Name &name) {
                std::vector<std::string> v;
                self->CheckStrippedStatus(name, v);
                return v;
        }
};

%pythoncode %{
class Database(_Database):
    def __init__(self, locale):
        _Database.__init__(self, locale)
        self._locale = locale

    def GetLocale(self):
        return self._locale
%}
