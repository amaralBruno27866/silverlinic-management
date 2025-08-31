-- Queries to detect duplicate records for migration
-- 1) Clients: duplicates by normalized email
-- 2) Clients: duplicates by firstname+lastname+normalized_phone
-- 3) Assessors: duplicates by normalized email
-- 4) Assessors: duplicates by firstname+lastname+normalized_phone

-- Clients by normalized email (returns rows for each member of duplicate groups)
SELECT lower(trim(email)) AS normalized_key, 'client' AS table_name, id, firstname, lastname, phone, email, created_at, modified_at
FROM client
WHERE email IS NOT NULL AND trim(email) <> ''
  AND lower(trim(email)) IN (
    SELECT lower(trim(email))
    FROM client
    WHERE email IS NOT NULL AND trim(email) <> ''
    GROUP BY lower(trim(email))
    HAVING COUNT(*) > 1
  )
ORDER BY normalized_key, id;

-- Clients by firstname+lastname+normalized_phone
SELECT lower(trim(firstname)) AS firstname_key, lower(trim(lastname)) AS lastname_key, replace(replace(phone,' ',''),'-','') AS phone_key,
       'client' AS table_name, id, firstname, lastname, phone, email, created_at, modified_at
FROM client
WHERE trim(firstname) <> '' AND trim(lastname) <> '' AND replace(replace(phone,' ',''),'-','') IS NOT NULL AND replace(replace(phone,' ',''),'-','') <> ''
  AND (lower(trim(firstname)), lower(trim(lastname)), replace(replace(phone,' ',''),'-','')) IN (
    SELECT lower(trim(firstname)), lower(trim(lastname)), replace(replace(phone,' ',''),'-','')
    FROM client
    WHERE trim(firstname) <> '' AND trim(lastname) <> ''
    GROUP BY lower(trim(firstname)), lower(trim(lastname)), replace(replace(phone,' ',''),'-','')
    HAVING COUNT(*) > 1
  )
ORDER BY firstname_key, lastname_key, phone_key, id;

-- Assessors by normalized email
SELECT lower(trim(email)) AS normalized_key, 'assessor' AS table_name, id, firstname, lastname, phone, email, created_at, modified_at
FROM assessor
WHERE email IS NOT NULL AND trim(email) <> ''
  AND lower(trim(email)) IN (
    SELECT lower(trim(email))
    FROM assessor
    WHERE email IS NOT NULL AND trim(email) <> ''
    GROUP BY lower(trim(email))
    HAVING COUNT(*) > 1
  )
ORDER BY normalized_key, id;

-- Assessors by firstname+lastname+normalized_phone
SELECT lower(trim(firstname)) AS firstname_key, lower(trim(lastname)) AS lastname_key, replace(replace(phone,' ',''),'-','') AS phone_key,
       'assessor' AS table_name, id, firstname, lastname, phone, email, created_at, modified_at
FROM assessor
WHERE trim(firstname) <> '' AND trim(lastname) <> '' AND replace(replace(phone,' ',''),'-','') IS NOT NULL AND replace(replace(phone,' ',''),'-','') <> ''
  AND (lower(trim(firstname)), lower(trim(lastname)), replace(replace(phone,' ',''),'-','')) IN (
    SELECT lower(trim(firstname)), lower(trim(lastname)), replace(replace(phone,' ',''),'-','')
    FROM assessor
    WHERE trim(firstname) <> '' AND trim(lastname) <> ''
    GROUP BY lower(trim(firstname)), lower(trim(lastname)), replace(replace(phone,' ',''),'-','')
    HAVING COUNT(*) > 1
  )
ORDER BY firstname_key, lastname_key, phone_key, id;
