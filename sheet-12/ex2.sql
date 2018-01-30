SELECT f0.subject, f3.object
FROM wikidata as f0, wikidata as f1, wikidata as f2, wikidata as f3
WHERE
f0.subject=f1.subject
AND f1.subject=f2.subject
AND f2.subject=f3.subject
AND f0.object="Germany"
AND f0.predicate="country of citizenship"
AND f1.object="computer scientist"
AND f1.predicate="occupation"
AND f2.object="female"
AND f2.predicate="sex or gender"
AND f3.predicate="educated at"
;