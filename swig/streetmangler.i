%module streetmangler
%include "std_string.i"
%include "std_vector.i"
%include "typemaps.i"

class StatusPart {
public:
        StatusPart(int priority, const std::string& full, const std::string& canonical, const std::string& abbrev, int flags);
        const std::string& GetFull() const;
        const std::string& GetCanonical() const;
        const std::string& GetAbbrev() const;
        int GetFlags() const;

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

        bool HasStatusPart() const;
        bool IsStatusPartAtLeft() const;
        bool IsStatusPartAtRight() const;
        int GetStatusFlags() const;

        std::string Join(int flags = 0) const;
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

#ifdef SWIGPYTHON
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
#endif

#ifdef SWIGPERL
%perlcode %{
use Encode;

package StreetMangler::Database;

sub new {
	my $pkg = shift;
	my ($locale) = @_;
	my $self = { _locale => $locale, _database => StreetMangler::_Database->new($locale) };
	bless $self, $pkg if defined($self);
}

sub Add {
	my ($self, $string) = @_;
	return $self->{_database}->Add(Encode::encode('utf-8', $string))
}

sub CheckExactMatch {
	my ($self, $string) = @_;
	return $self->{_database}->CheckExactMatch(Encode::encode('utf-8', $string))
}

sub CheckCanonicalForm {
	my ($self, $string) = @_;
	return [ map { Encode::decode('utf-8', $_) } @{ $self->{_database}->CheckCanonicalForm(Encode::encode('utf-8', $string)) } ];
}

sub CheckSpelling {
	my ($self, $string) = @_;
	return [ map { Encode::decode('utf-8', $_) } @{ $self->{_database}->CheckSpelling(Encode::encode('utf-8', $string)) } ];
}

sub CheckStrippedStatus {
	my ($self, $string) = @_;
	return [ map { Encode::decode('utf-8', $_) } @{ $self->{_database}->CheckStrippedStatus(Encode::encode('utf-8', $string)) } ];
}

package StreetMangler::Name;

use vars qw(@ISA);
@ISA = qw( StreetMangler::_Name );

use overload '""' => \&Join;

sub new {
	my $pkg = shift;
	my ($name, $locale) = @_;
	my $self = StreetMangler::_Name->new(Encode::encode('utf-8', $name), $locale);
	bless $self, $pkg if defined($self);
}

sub Join {
	my ($self, $flags) = @_;
	$flags = 0 unless defined $flags;
	return Encode::decode('utf-8', StreetMangler::_Name::Join($self, $flags));
}

%}
#endif
