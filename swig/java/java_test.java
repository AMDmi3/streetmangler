/*
 vim: sts=4 sw=4 et
 */

import org.openstreetmap.StreetMangler.*;
import static org.junit.Assert.*;

class TestCase {
    static {
        System.loadLibrary("StreetMangler");
    }

    public static Locale locale = new Locale("ru_RU");
    public static String street = "улица Ленина";
    public static String[] street_list = {"улица Ленина", "Ленина улица", "ул. Ленина"};

    public static void main(String[] args) {
        test_exact();
        test_canonical();
        test_spelling();
    }

    public static void compareList(Iterable<String> i, String[] l) {
        int idx = 0;
        for (String s: i) assertEquals(s, l[idx++]);
    }

    @org.junit.Test
    public static void test_canonical() {
        Database db = new Database(locale);
	assertTrue("Canonical name not empty", db.CheckCanonicalForm(street).isEmpty());
	db.Add(street);
        String[] check = {street};
	for (String n: street_list) {
            compareList(db.CheckCanonicalForm(n), check);
            compareList(db.CheckCanonicalForm(new Name(n, locale)), check);
        }
    }

    @org.junit.Test
    public static void test_spelling() {
        Database db = new Database(locale);
        String[] check = {street};
	String bogus = "улица Лемина";
	assertTrue("Spelling list not empty", db.CheckSpelling(street).isEmpty());
	db.Add(street);
        for (String n: street_list) {
            compareList(db.CheckSpelling(n), check);
            compareList(db.CheckSpelling(new Name(n, locale)), check);
        }
        compareList(db.CheckSpelling(bogus), check);
        compareList(db.CheckSpelling(new Name(bogus, locale)), check);
    }

    @org.junit.Test
    public static void test_exact() {
        Database db = new Database(locale);
	assertFalse(db.CheckExactMatch(street));
	db.Add(street);
	assertTrue(db.CheckExactMatch(street));
	assertTrue(db.CheckExactMatch(new Name(street, locale)));
	assertFalse(db.CheckExactMatch("Ленина улица"));
	assertFalse(db.CheckExactMatch("ул Ленина"));
    }

    @org.junit.Test
    public static void test_string_vector() {
        StringVector v = new StringVector();
        String[] sv = {"xxx", "yyy"};
        for (String s: sv) v.add(s);
        for (String s: v)
            System.out.println(s);
    }
}
