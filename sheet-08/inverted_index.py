"""
Copyright 2017, University of Freiburg
Chair of Algorithms and Data Structures.
Hannah Bast <bast@cs.uni-freiburg.de>
Claudius Korzen <korzen@cs.uni-freiburg.de>
"""

import math
import re
import sys

import numpy

import scipy.sparse

DEFAULT_B = 0.75
DEFAULT_K = 1.75


class InvertedIndex:
    """
    A simple inverted index that uses BM25 scores.
    """

    def __init__(self):
        """
        Creates an empty inverted index.
        """
        self.inverted_lists = {}  # The inverted lists.
        self.docs = []  # The docs, each in form (title, description).
        self.doc_lengths = []  # The document lengths (= number of words).
        self.terms = []  # List of all words (unque one) (=inverted_list.first)
        self.inv_terms = {}  # (term, index in self.term)
        self.A = []  # Term-dcument matrix
        self.num_docs = 0  # Number of docs

    def read_from_file(self, file_name, b=None, k=None):
        """
        Construct the inverted index from the given file. The expected format
        of the file is one document per line, in the format
        <title>TAB<description>. Each entry in the inverted list associated to
        a word should contain a document id and a BM25 score. Compute the BM25
        scores as follows:

        (1) In a first pass, compute the inverted lists with tf scores (that
            is the number of occurrences of the word within the <title> and the
            <description> of a document). Further, compute the document length
            (DL) for each document (that is the number of words in the <title>
            and the <description> of a document). Afterwards, compute the
            average document length (AVDL).
        (2) In a second pass, iterate each inverted list and replace the tf
            scores by BM25 scores, defined as:
            BM25 = tf * (k+1) / (k * (1 - b + b * DL/AVDL) + tf) * log2(N/df),
            where N is the total number of documents and df is the number of
            documents that contains the word.

        On reading the file, use UTF-8 as the standard encoding. To split the
        texts into words, use the method introduced in the lecture. Make sure
        that you ignore empty words.

        >>> ii = InvertedIndex()
        >>> ii.read_from_file("example.txt", b=0, k=float("inf"))
        >>> inv_lists = sorted(ii.inverted_lists.items())
        >>> [(w, [(i, '%.3f' % tf) for i, tf in l]) for w, l in inv_lists]
        ... # doctest: +NORMALIZE_WHITESPACE
        [('animated', [(1, '0.415'), (2, '0.415'), (4, '0.415')]),
         ('animation', [(3, '2.000')]),
         ('film', [(2, '1.000'), (4, '1.000')]),
         ('movie', [(1, '0.000'), (2, '0.000'), (3, '0.000'), (4, '0.000')]),
         ('non', [(2, '2.000')]),
         ('short', [(3, '1.000'), (4, '2.000')])]

        >>> ii = InvertedIndex()
        >>> ii.read_from_file("example.txt", b=0.75, k=1.75)
        >>> inv_lists = sorted(ii.inverted_lists.items())
        >>> [(w, [(i, '%.3f' % tf) for i, tf in l]) for w, l in inv_lists]
        ... # doctest: +NORMALIZE_WHITESPACE
        [('animated', [(1, '0.459'), (2, '0.402'), (4, '0.358')]),
         ('animation', [(3, '2.211')]),
         ('film', [(2, '0.969'), (4, '0.863')]),
         ('movie', [(1, '0.000'), (2, '0.000'), (3, '0.000'), (4, '0.000')]),
         ('non', [(2, '1.938')]),
         ('short', [(3, '1.106'), (4, '1.313')])]
        """

        b = DEFAULT_B if b is None else b
        k = DEFAULT_K if k is None else k

        # First pass: Compute (1) the inverted lists with tf scores and (2) the
        # document lengths.
        with open(file_name, "r", encoding="utf-8") as f:
            doc_id = 0
            for line in f:
                line = line.strip()

                dl = 0  # Compute the document length (number of words).
                doc_id += 1

                for word in re.split("[^A-Za-z]+", line):
                    word = word.lower().strip()

                    # Ignore the word if it is empty.
                    if len(word) == 0:
                        continue

                    dl += 1

                    if word not in self.inverted_lists:
                        # The word is seen for first time, create new list.
                        self.inverted_lists[word] = [(doc_id, 1)]
                        continue

                    # Get last posting to check if the doc was already seen.
                    last = self.inverted_lists[word][-1]
                    if last[0] == doc_id:
                        # The doc was already seen, increment tf by 1.
                        self.inverted_lists[word][-1] = (doc_id, last[1] + 1)
                    else:
                        # The doc was not already seen, set tf to 1.
                        self.inverted_lists[word].append((doc_id, 1))

                # Store the doc as a tuple (title, description).
                self.docs.append(tuple(line.split("\t")))

                # Register the document length.
                self.doc_lengths.append(dl)

        # Compute N (the total number of documents).
        n = len(self.docs)
        # Compute AVDL (the average document length).
        avdl = sum(self.doc_lengths) / n

        # Second pass: Iterate the inverted lists and replace the tf scores by
        # BM25 scores, defined as follows:
        # BM25 = tf * (k + 1) / (k * (1 - b + b * DL / AVDL) + tf) * log2(N/df)
        for word, inverted_list in self.inverted_lists.items():
            for i, (doc_id, tf) in enumerate(inverted_list):
                # Obtain the document length (dl) of the document.
                dl = self.doc_lengths[doc_id - 1]  # doc_id is 1-based.
                # Compute alpha = (1 - b + b * DL / AVDL).
                alpha = 1 - b + (b * dl / avdl)
                # Compute tf2 = tf * (k + 1) / (k * alpha + tf).
                tf2 = tf * (1 + (1 / k)) / (alpha + (tf / k)) if k > 0 else 1
                # Compute df (that is the length of the inverted list).
                df = len(self.inverted_lists[word])
                # Compute the BM25 score = tf' * log2(N/df).
                inverted_list[i] = (doc_id, tf2 * math.log(n / df, 2))
        self.num_docs = n

    def render_output(self, postings, keywords, k=3):
        """
        Render the output for the top-k of the given postings. Fetch the
        the titles and descriptions of the related docs and highlight the
        occurences of the given keywords in the output, using ANSI escape
        codes.
        """

        # Compile a pattern to identify the given keywords in a string.
        p = re.compile('\\b(' + '|'.join(keywords) + ')\\b', re.IGNORECASE)

        # Output at most k matching docs.
        for i in range(min(len(postings), k)):
            doc_id, tf = postings[i]
            title, desc = self.docs[doc_id - 1]  # doc_id is 1-based.

            # Highlight the keywords in the title in bold and red.
            title = re.sub(p, "\033[0m\033[1;31m\\1\033[0m\033[1m", title)

            # Print the rest of the title in bold.
            title = "\033[1m%s\033[0m" % title

            # Highlight the keywords in the description in red.
            desc = re.sub(p, "\033[31m\\1\033[0m", desc)

            print("\n%s\n%s" % (title, desc))

        print("\n# total hits: %s." % len(postings))

    def preprocessing_vsm(self, l2normalize=False, l1normalize=False,
                          l3normalize=False):
        """
        Compute the sparse term-document matrix from the inverted list
        >>> ii = InvertedIndex()
        >>> ii.read_from_file("example.txt", b=0, k=float("inf"))
        >>> ii.preprocessing_vsm(l2normalize=False)
        >>> res = sorted(ii.A.todense().tolist())
        >>> import pprint
        >>> pprint.pprint([["%.3f" % x for x in items] for items in res])
        [['0.000', '0.000', '0.000', '0.000'],
         ['0.000', '0.000', '1.000', '2.000'],
         ['0.000', '0.000', '2.000', '0.000'],
         ['0.000', '1.000', '0.000', '1.000'],
         ['0.000', '2.000', '0.000', '0.000'],
         ['0.415', '0.415', '0.000', '0.415']]

        >>> ii2 = InvertedIndex()
        >>> ii2.read_from_file("example.txt", b=0.75, k=1.75)
        >>> ii2.preprocessing_vsm(l2normalize=True)
        >>> res = sorted(ii2.A.todense().tolist())
        >>> import pprint
        >>> pprint.pprint([["%.3f" % x for x in items] for items in res])
        [['0.000', '0.000', '0.000', '0.000'],
         ['0.000', '0.000', '0.447', '0.815'],
         ['0.000', '0.000', '0.894', '0.000'],
         ['0.000', '0.440', '0.000', '0.535'],
         ['0.000', '0.879', '0.000', '0.000'],
         ['1.000', '0.182', '0.000', '0.222']]

         >>> ii3 = InvertedIndex()
         >>> ii3.read_from_file("example.txt", b=0.75, k=1.75)
         >>> ii3.preprocessing_vsm(l1normalize=True)
         >>> res = sorted(ii3.A.todense().tolist())
         >>> import pprint
         >>> pprint.pprint([["%.3f" % x for x in items] for items in res])
         [['0.000', '0.000', '0.000', '0.000'],
          ['0.000', '0.000', '0.333', '0.518'],
          ['0.000', '0.000', '0.667', '0.000'],
          ['0.000', '0.293', '0.000', '0.340'],
          ['0.000', '0.586', '0.000', '0.000'],
          ['1.000', '0.122', '0.000', '0.141']]


         >>> ii4 = InvertedIndex()
         >>> ii4.read_from_file("example.txt", b=0.75, k=1.75)
         >>> ii4.preprocessing_vsm(l3normalize=True)
         >>> res = sorted(ii4.A.todense().tolist())
         >>> import pprint
         >>> pprint.pprint([["%.3f" % x for x in items] for items in res])
         [['0.000', '0.000', '0.000', '0.000'],
          ['0.000', '0.000', '0.481', '0.915'],
          ['0.000', '0.000', '0.961', '0.000'],
          ['0.000', '0.479', '0.000', '0.601'],
          ['0.000', '0.959', '0.000', '0.000'],
          ['1.000', '0.199', '0.000', '0.250']]
        """

        for idx, word in enumerate(self.inverted_lists):
            self.terms.append(word)
            self.inv_terms[word] = idx

        row = []  # i-th row represents term[i]
        col = []
        data = []

        for row_no, term in enumerate(self.terms):
            for col_no, bm25 in self.inverted_lists[term]:
                row.append(row_no)
                col.append(col_no - 1)
                data.append(bm25)
        self.A = scipy.sparse.csr_matrix((data, (row, col)),
                                         shape=(len(self.terms),
                                                self.num_docs))

        # Matrix normalization
        # If more than one normalization specified,
        # the one with lower x # is chosen

        if l1normalize:
            v = 1.0 / self.A.sum(axis=0)
            v = scipy.sparse.csr_matrix(v)
            self.A = self.A.multiply(v)
            return

        if l2normalize:
            v = self.A.multiply(self.A).sum(axis=0)
            v = 1.0 / numpy.sqrt(v)
            # Without conbersion I get `Memory Error`
            v = scipy.sparse.csr_matrix(v)
            self.A = self.A.multiply(v)
            return

        if l3normalize:
            v = self.A.multiply(self.A).multiply(self.A).sum(axis=0)
            v = 1.0 / numpy.power(v, 1.0/3.0)
            v = scipy.sparse.csr_matrix(v)
            self.A = self.A.multiply(v)

    def process_query_vsm(self, query, use_refinements=False):
        """
        Proces a given query

        >>> ii = InvertedIndex()
        >>> ii.inverted_lists = {
        ... "foo": [(1, 0.2), (3, 0.6)],
        ... "bar": [(1, 0.4), (2, 0.7), (3, 0.5)],
        ... "baz": [(2, 0.1)]}
        >>> ii.num_docs = 3
        >>> ii.preprocessing_vsm()
        >>> res = ii.process_query_vsm(["foo", "bar"], use_refinements=False)
        >>> [(id, "%.1f" % tf) for id, tf in res]
        [(3, '1.1'), (2, '0.7'), (1, '0.6')]

        >>> ii2 = InvertedIndex()
        >>> ii2.inverted_lists = {
        ... "foo": [(1, 0.2), (3, 0.6)],
        ... "bar": [(2, 0.4), (3, 0.1), (4, 0.8)]
        ... }
        >>> ii2.num_docs = 4
        >>> ii2.preprocessing_vsm(l2normalize=False)
        >>> res = ii2.process_query_vsm(["foo", "bar", "foo", "bar"])
        >>> [(id, "%.1f" % tf) for id, tf in res]
        [(4, '1.6'), (3, '1.4'), (2, '0.8'), (1, '0.4')]

        """

        query_vec = [0] * len(self.terms)
        for q in query:
            query_vec[self.inv_terms[q]] += 1

        q = scipy.sparse.csr_matrix(query_vec)
        q = q.dot(self.A)
        q = q.toarray()[0]
        q = [(i+1, j) for i, j in list(enumerate(q))]
        q = [x for x in q if x[1] != 0]
        return sorted(q, key=lambda x: x[1], reverse=True)


if __name__ == "__main__":
    # Parse the command line arguments.
    if len(sys.argv) < 2:
        print("Usage: python3 inverted_index.py <file> [<b>] [<k>]")
        sys.exit()

    file_name = sys.argv[1]
    b = float(sys.argv[2]) if len(sys.argv) > 2 else None
    k = float(sys.argv[3]) if len(sys.argv) > 3 else None

    # Create a new inverted index from the given file.
    print("Reading from file '%s'." % file_name)

    ii = InvertedIndex()
    ii.read_from_file(file_name, b=b, k=k)
    print("... done!")
    print("Preprocessing...")
    ii.preprocessing_vsm(l2normalize=True)
    print("... done!")

    while True:
        # Ask the user for a keyword query.
        query = input("\nYour keyword query: ")

        # Split the query into keywords.
        keywords = [x.lower().strip() for x in re.split("[^A-Za-z]+", query)]

        # Process the keywords.
        postings = ii.process_query(keywords)

        # Render the output (with ANSI codes to highlight the keywords).
        ii.render_output(postings, keywords)
