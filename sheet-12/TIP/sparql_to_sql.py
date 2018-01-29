"""
Copyright 2018, University of Freiburg,
Chair of Algorithms and Data Structures.
Authors: Patrick Brosi <brosi@cs.uni-freiburg.de>,
         Claudius Korzen <korzen@cs.uni-freiburg.de>.
"""

import re
import readline  # NOQA
import sys


class SPARQL:
    """ A simple SPARQL engine for a SQLite backend. """

    def __init__(self):
        """
        Creates a new SPARQL engine.
        """
        # A pattern to parse the triples in the WHERE clause of a SPARQL query.
        # Assumes that all strings are surrounded by quotation marks (").
        self.triple_pattern = re.compile(
            '\s?(\?[^\s]+|"[^"]+")\s+("[^"]+")\s+(\?[^\s]+|"[^"]+")'
        )

    def sparql_to_sql(self, sparql):
        """
        Translates the given SPARQL query to a corresponding SQL query.

        TODO: Implement test case(s).
        # >>> sts = SPARQL()
        # >>> sts.sparql_to_sql("SELECT ?f ?p WHERE {" \
        # "?f \\"instance of\\" \\"film\\" ." \
        # "?f \\"award received\\" \\"Academy Award for Best Picture\\" ." \
        # "?f \\"director\\" ?p ." \
        # "?p \\"country of citizenship\\" \\"Germany\\"}")
        >>> sts = SPARQL()
        ... # doctest: +NORMALIZE_WHITESPACE
        >>> sts.sparql_to_sql("SELECT ?p1 ?p2 ?f WHERE {" \
                "?p1 \\"acted_in\\" ?f ." \
                "?p2 \\"acted_in\\" ?f ." \
                "?p1 \\"married_to\\" ?p2 }")
        'SELECT p1, p2, f \
FROM wikidata as f0, \
wikidata as f1, \
wikidata as f2 \
WHERE f0.object = f1.object \
AND f0.subject = f2.subject \
AND f1.subject = f2.object'
        """
        # Transform all letters to lower cases.
        sparqll = sparql.lower()

        # Find all variables in the SPARQL between the SELECT and WHERE clause.
        select_start = sparqll.find("select ") + 7
        select_end = sparqll.find(" where", select_start)
        variables = sparql[select_start:select_end].split()

        # Find all triples between "WHERE {" and "}"
        where_start = sparqll.find("{", select_end) + 1
        where_end = sparqll.rfind("}", where_start)
        where_text = sparql[where_start:where_end]
        triple_texts = where_text.split(".")
        triples = []
        for triple_text in triple_texts:
            m = self.triple_pattern.match(triple_text)
            subj = m.group(1).strip('"')
            pred = m.group(2).strip('"')
            obj = m.group(3).strip('"')
            triples.append((subj, pred, obj))

        # Find the (optional) LIMIT clause.
        limit_start = sparqll.find(" limit ", where_end)
        limit = sparql[limit_start + 7:].strip() if limit_start > 0 else None

        # TODO: Compose the SQL query and return it.

        # Compose SELECT clause
        select_clause = "SELECT "
        for v in variables:
            select_clause += v[1:] + ", "
        # Remove last comma
        select_clause = select_clause[:-2]

        # Compose FROM clause and generate variable occurences map
        from_clause = "FROM "
        var_occurrences = {key: [] for key in variables}

        for i, (subj, pred, obj) in enumerate(triples):
            # Expand "FROM" clause
            from_clause += "wikidata as f" + str(i) + ", "

            # I asasume that only variables start with '?'
            if subj in var_occurrences:
                var_occurrences[subj].append("f" + str(i) + ".subject")

            if obj in var_occurrences:
                var_occurrences[obj].append("f" + str(i) + ".object")

        # Remove last comma from from_clause:
        from_clause = from_clause[:-2]

        # Compose WHERE clause
        where_clause = "WHERE "

        # sorted() in needed only for test coherency
        for key, val in sorted(var_occurrences.items()):
            for i in range(1, len(val)):
                where_clause += val[i-1] + " = " + val[i] + " AND "
        where_clause = where_clause[:-5]

        return select_clause + ' ' + from_clause + ' ' + where_clause

    def process_sql_query(self, db_name, sql):
        """
        Runs the given SQL query against the given instance of a SQLite3
        database and returns the result rows.

        TODO: Implement test case(s).
        """
        # TODO: Run the SQL query against the database and return the result.
        return None


if __name__ == '__main__':
    # TODO: Read the path to the SQLite3 database from the command line.

    engine = SPARQL()

    while (True):
        # Read the SPARQL query to process from the command line.
        sparql = input("Enter SPARQL query: ")

        # Translate the SPARQL query to an SQL query.
        try:
            sql = engine.sparql_to_sql(sparql)
        except Exception:
            print("Syntax error...")
            sys.exit(1)

        # TODO: Run the SQL query against the database.
        # TODO: Output the result rows.
