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

#ifdef SWIGPYTHON
%typemap(out) std::vector<std::string> {
	$result = PyList_New($1.size());
	for (int i = 0; i < $1.size(); i++) {
		PyObject *o = PyString_FromString($1[i].c_str());
		PyList_SetItem($result, i, o);
	}
}
#endif

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
