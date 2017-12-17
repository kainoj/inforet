"""
Copyright 2017, University of Freiburg
Chair of Algorithms and Data Structures.
Claudius Korzen <korzen@cs.uni-freiburg.de>
"""

import re
import sys

from inverted_index import InvertedIndex


class EvaluateInvertedIndex:
    """
    Class for evaluating the InvertedIndex class against a benchmark.
    """

    def read_benchmark(self, file_name):
        """
        Read a benchmark from the given file. The expected format of the file
        is one query per line, with the ids of all documents relevant for that
        query, like: <query>TAB<id1>WHITESPACE<id2>WHITESPACE<id3> ...

        >>> evaluate = EvaluateInvertedIndex()
        >>> benchmark = evaluate.read_benchmark("example-benchmark.txt")
        >>> sorted(benchmark.items())
        [('animated film', {1, 3, 4}), ('short film', {3, 4})]
        """
        benchmark = {}

        with open(file_name, "r", encoding="utf-8") as f:
            for line in f:
                # Split the line into the query and the groundtruth part.
                query, gt = line.strip().split('\t')
                # Split the groundtruth part into ids and store them as a set.
                benchmark[query] = {int(x) for x in gt.split(" ")}

        return benchmark

    def evaluate(self, ii, benchmark, use_refinements=False, verbose=True):
        """
        Evaluate the given inverted index against the given benchmark as
        follows. Process each query in the benchmark with the given inverted
        index and compare the result list with the groundtruth in the
        benchmark. For each query, compute the measure P@3, P@R and AP as
        explained in the lecture. Aggregate the values to the three mean
        measures MP@3, MP@R and MAP and return them.

        Implement a parameter 'use_refinements' that controls the use of
        ranking refinements on calling the method process_query of your
        inverted index.

        >>> ii = InvertedIndex()
        >>> ii.read_from_file("example.txt", b=0.75, k=1.75)
        >>> ii.preprocessing_vsm()
        >>> evaluator = EvaluateInvertedIndex()
        >>> benchmark = evaluator.read_benchmark("example-benchmark.txt")
        >>> measures = evaluator.evaluate(ii, benchmark, use_refinements=False,
        ... verbose=False)
        >>> [round(x, 3) for x in measures]
        [0.667, 0.833, 0.694]
        """
        sum_p_at_3 = 0
        sum_p_at_r = 0
        sum_ap = 0

        num_queries = len(benchmark)

        for query, relevant_ids in benchmark.items():
            if verbose:
                print("Processing query '%s' ..." % query)

            # Process the query by the index and fetch only the document ids.
            words = [x.lower().strip() for x in re.split("[^A-Za-z]+", query)]
            ids = [x[0] for x in ii.process_query_vsm(words, use_refinements)]

            # Compute P@3.
            p_at_3 = self.precision_at_k(ids, relevant_ids, 3)
            sum_p_at_3 += p_at_3
            if verbose:
                print("  P@3: %.2f" % p_at_3)

            # Compute P@R.
            r = len(relevant_ids)
            p_at_r = self.precision_at_k(ids, relevant_ids, r)
            sum_p_at_r += p_at_r
            if verbose:
                print("  P@R: %.2f" % p_at_r)

            # Compute AP.
            ap = self.average_precision(ids, relevant_ids)
            sum_ap += ap
            if verbose:
                print("  AP: %.2f" % ap)

        # Compute MP@3.
        mp_at_3 = sum_p_at_3 / num_queries
        # Compute MP@R.
        mp_at_r = sum_p_at_r / num_queries
        # Compute MAP.
        map_value = sum_ap / num_queries

        return mp_at_3, mp_at_r, map_value

    def precision_at_k(self, result_ids, relevant_ids, k):
        """
        Compute the measure P@k for the given list of result ids as it was
        returned by the inverted index for a single query, and the given set of
        relevant document ids.

        Note that the relevant document ids are 1-based (as they reflect the
        line number in the dataset file).

        >>> evaluator = EvaluateInvertedIndex()
        >>> evaluator.precision_at_k([5, 3, 6, 1, 2], {1, 2, 5, 6, 7, 8}, k=2)
        0.5
        >>> evaluator.precision_at_k([5, 3, 6, 1, 2], {1, 2, 5, 6, 7, 8}, k=4)
        0.75
        """
        if k == 0:
            return 0

        num_relevant_result_ids = 0
        for i in range(0, min(len(result_ids), k)):
            if result_ids[i] in relevant_ids:
                num_relevant_result_ids += 1
        return num_relevant_result_ids / k

    def average_precision(self, result_ids, relevant_ids):
        """
        Compute the average precision (AP) for the given list of result ids as
        it was returned by the inverted index for a single query, and the given
        set of relevant document ids.

        Note that the relevant document ids are 1-based (as they reflect the
        line number in the dataset file).

        >>> evaluator = EvaluateInvertedIndex()
        >>> evaluator.average_precision([7, 17, 9, 42, 5], {5, 7, 12, 42})
        0.525
        """
        sum_ap = 0
        for i in range(0, len(result_ids)):
            if result_ids[i] in relevant_ids:
                sum_ap += self.precision_at_k(result_ids, relevant_ids, i + 1)
        return sum_ap / len(relevant_ids)


if __name__ == "__main__":
    # Parse the command line arguments.
    if len(sys.argv) < 3:
        print("""Usage: python3 evaluate_inverted_index.py """
              """<file> <benchmark> [<b>] [<k>]""")
        sys.exit()

    file_name = sys.argv[1]
    benchmark_file_name = sys.argv[2]
    b = float(sys.argv[3]) if len(sys.argv) > 3 else None
    k = float(sys.argv[4]) if len(sys.argv) > 4 else None

    # Create a new inverted index from the given file.
    print("Creating index with BM25 scores from file '%s'..." % file_name)
    index = InvertedIndex()
    index.read_from_file(file_name, b=b, k=k)
    index.preprocessing_vsm(l3normalize=True)

    # Read the benchmark.
    print("Reading benchmark from file '%s'..." % benchmark_file_name)
    evaluator = EvaluateInvertedIndex()
    benchmark = evaluator.read_benchmark(benchmark_file_name)

    # Evaluate the the inverted index against the benchmark.
    measures = evaluator.evaluate(index, benchmark)

    # Output MP@3, MP@R and MAP.
    print("Mean results:")
    print("  MP@3: %s" % round(measures[0], 3))
    print("  MP@R: %s" % round(measures[1], 3))
    print("  MAP:  %s" % round(measures[2], 3))
