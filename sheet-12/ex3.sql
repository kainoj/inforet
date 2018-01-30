SELECT f0.subject
FROM wikidata as f0, wikidata as f1
WHERE
f0.subject=f1.subject
AND f0.object="video game"
AND f0.predicate="instance of"
AND f1.object="PlayStation 4"
AND f1.predicate="platform"
LIMIT 10;
