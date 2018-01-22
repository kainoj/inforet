"""
Copyright 2018, University of Freiburg.

Elmar Haussmann <haussmann@cs.uni-freiburg.de>
Patrick Brosi <brosi@cs.uni-freiburg.de>
Claudius Korzen <korzen@cs.uni-freiburg.de>
"""

import re
import sys

import numpy

from scipy.sparse import csr_matrix


def generate_vocabularies(filename):
    """
    Reads the given file and generates vocabularies mapping from label/class to
    label ids and from word to word id.

    You should call this ONLY on your training data.
    """

    # Map from label/class to label id.
    class_vocabulary = dict()

    # Map from word to word id.
    word_vocabulary = dict()

    class_id = 0
    word_id = 0

    # Read the file (containing the training data).
    with open(filename, "r") as f:
        for line in f:
            label, text = line.strip().split('\t')

            if label not in class_vocabulary:
                class_vocabulary[label] = class_id
                class_id += 1

            # Remove all non-characters and non-digits from the text.
            text = re.sub("\W+", " ", text.lower())
            # Split the text into words.
            words = text.split()

            # Add the words to the vocabulary.
            for word in words:
                if word not in word_vocabulary:
                    word_vocabulary[word] = word_id
                    word_id += 1

    return word_vocabulary, class_vocabulary


def read_labeled_data(filename, class_vocab, word_vocab):
    """
    Reads the given file and returns a sparse document-term matrix as well as a
    list of labels of each document. You need to provide a class and word
    vocabulary. Words not in the vocabulary are ignored. Documents labeled
    with classes not in the class vocabulary are also ignored.

    The returned document-term matrix X has size n x m, where n is the number
    of documents and m the number of word ids. The value at i, j denotes the
    number of times word id j is present in document i.

    The returned labels vector y has size n (one label for each document). The
    value at index j denotes the label (class id) of document j.
    """

    labels = []
    row, col, value = [], [], []
    num_examples = 0
    num_cols = len(word_vocab)

    with open(filename, "r") as f:
        for i, line in enumerate(f):
            label, text = line.strip().split('\t')

            if label in class_vocab:
                num_examples += 1
                labels.append(class_vocab[label])
                words = re.sub("\W+", " ", text.lower()).split()
                for w in words:
                    if w in word_vocab:
                        w_id = word_vocab[w]
                        row.append(i)
                        col.append(w_id)
                        # Duplicate values at the same position i,j are summed.
                        value.append(1.0)

    x = csr_matrix((value, (row, col)), shape=(num_examples, num_cols))
    y = numpy.array(labels)
    return x, y


class NaiveBayes(object):
    """
    A simple naive bayes classifier as explained in the lecture.

    >>> numpy.set_printoptions(formatter={"float": lambda x: ("%.3f" % x)})
    """

    def __init__(self):
        """
        Creates a new naive bayes classifier supporting num_classes of classes
        and num_features of words.
        """
        # The stored probabilities of each class.
        self.p_c = None
        # The stored probabilities of each word in each class
        self.p_wc = None
        # Epsilon (smoothing)
        self.EPS = 0.1

    def train(self, x, y):
        """
        Trains on the sparse document-term matrix X and associated labels y.

        In the test case below, p_wc is a class-term-matrix and has a row
        for each class and a column for each term. So the value at i,j is
        the p_wc for the j-th term in the i-th class.

        p_c is an array of global probabilities for each class.

        Remember to use epsilon = 1/10 for your training, as described in the
        lecture!

        >>> wv, cv = generate_vocabularies("example_train.tsv")
        >>> X, y = read_labeled_data("example_train.tsv", cv, wv)
        >>> nb = NaiveBayes()
        >>> nb.train(X, y)
        >>> numpy.round(numpy.exp(nb.p_wc), 3)
        array([[0.664, 0.336],
               [0.335, 0.665]])
        >>> numpy.round(numpy.exp(nb.p_c), 3)
        array([0.500, 0.500])
        """

        # Calculate p_c
        unique, counts = numpy.unique(y, return_counts=True)
        self.p_c = numpy.log(counts / len(y))

        # Compute p_wc
        # cv[i][j] == 1 iif doc j is in class i
        cv = [[1 if i == j else 0 for j in y] for i in range(0, max(y) + 1)]

        # Number of occurences of word i in documents in T_j (n_wc[i][j])
        n_wc = cv*x

        # Number of vocabulary
        nmbr_v = x.shape[1]

        # Inverse nubmer of occurences of all words in document from T_c
        n_c = numpy.diag(numpy.reciprocal(n_wc.sum(1) + self.EPS * nmbr_v))

        # Smooth
        self.p_wc = numpy.log(n_c.dot(n_wc + self.EPS))

    def predict(self, x):
        """
        Predicts a label for each example in the document-term matrix,
        based on the learned probabities stored in this class.

        Returns a list of predicted label ids.

        >>> wv, cv = generate_vocabularies("example_train.tsv")
        >>> X, y = read_labeled_data("example_train.tsv", cv, wv)
        >>> nb = NaiveBayes()
        >>> nb.train(X, y)
        >>> X_test, y_test = read_labeled_data("example_test.tsv", cv, wv)
        >>> nb.predict(X_test)
        array([0, 1, 0])
        >>> nb.predict(X)
        array([0, 0, 1, 0, 1, 1])
        """

        res = x.dot(self.p_wc.transpose())
        res = res + self.p_c  # seems like cololumn wise addition
        return numpy.array([numpy.argmax(row) for row in res])

    def evaluate(self, x, y):
        """
        Predicts the labels of X and computes the precisions, recalls and
        F1 scores for each class.

        >>> wv, cv = generate_vocabularies("example_train.tsv")
        >>> X_train, y_train = read_labeled_data("example_train.tsv", cv, wv)
        >>> X_test, y_test = read_labeled_data("example_test.tsv", cv, wv)
        >>> nb = NaiveBayes()
        >>> nb.train(X_train, y_train)
        >>> precisions, recalls, f1_scores = nb.evaluate(X_test, y_test)
        >>> precisions
        {0: 0.5, 1: 1.0}
        >>> recalls
        {0: 1.0, 1: 0.5}
        >>> {x: '%.2f' % f1_scores[x] for x in f1_scores}
        {0: '0.67', 1: '0.67'}
        """
        classified = self.predict(x)

        # Number of classes
        c = numpy.max(y) + 1

        # Number of documents labeled c
        dc = [(y == i).sum() for i in range(c)]

        # Dꞌc = number of documents classified as c
        dcprim = [(classified == i).sum() for i in range(c)]

        # Number of document classified AND labeled as c
        dnd = c * [0]
        for i in range(0, len(y)):
            if y[i] == classified[i]:
                dnd[y[i]] = dnd[y[i]] + 1

        # P ≔ |Dꞌc ∩ Dc| / |Dꞌc|
        p = {i: x/y for i, (x, y) in zip(range(c), zip(dnd, dcprim))}

        # R ≔ |Dꞌc ∩ Dc| / |Dc|
        r = {i: x/y for i, (x, y) in zip(range(c), zip(dnd, dc))}

        # F ≔ 2 · P · R / (P + R)
        f = {i: 2*p[i]*r[i] / (p[i]+r[i]) for i in range(c)}

        return p, r, f


if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("Usage: python3 naive_bayes.py <train-input> <test-input>")
        exit(1)

    train_input = sys.argv[1]
    test_input = sys.argv[2]

    word_vocab, class_vocab = generate_vocabularies(train_input)
    X_train, y_train = read_labeled_data(train_input, class_vocab, word_vocab)
    X_test, y_test = read_labeled_data(test_input, class_vocab, word_vocab)

    nb = NaiveBayes()

    # Train on the training dataset.
    nb.train(X_train, y_train)

    # Predict labels for the test dataset.
    predicted = nb.predict(X_test)

    # Output the precision, recall, and F1 score on the test data, for
    # each class separately as well as the (unweighted) average over all
    # classes.
    precisions, recalls, f1_scores = nb.evaluate(X_test, y_test)
    print("id \t| P \t| R \t| F \t|")
    for i, val in precisions.items():
        print("%d \t| %.2f \t| %.2f \t| %.2f \t|" % (i, val,
                                                     recalls[i], f1_scores[i]))

    print("F-mean = %.2f%%" % (100 * sum(f1_scores.values()) / len(f1_scores)))

    # TODO: Print the 30 words with the highest p_wc values per class which do
    #       not appear in the stopwords.txt provided on the Wiki.
