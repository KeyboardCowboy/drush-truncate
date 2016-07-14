# Drush Truncate

Simple drush utility to truncate database tables.

## Truncate a single table:

	$ drush truncate watchdog
	watchdog
	Truncate 1 table on yourdb? (y/n): 

## Truncate multiple tables:

	# Use an asterisk as a wildcard.
	$ drush truncate cache_entity_*
	cache_entity_comment
	cache_entity_node
	cache_entity_taxonomy_term
	cache_entity_taxonomy_vocabulary
	cache_entity_user
	Truncate 15 tables on yourdb? (y/n):

## Interactive mode lets you pick and choose.

	$ drush truncate --interactive cache_entity_*
	cache_entity_comment
	cache_entity_node
	cache_entity_taxonomy_term
	cache_entity_taxonomy_vocabulary
	cache_entity_user
	Truncate yourdb.cache_entity_comment? (y/n): n
	Skipped yourdb.cache_entity_comment.
	Truncate yourdb.cache_entity_node? (y/n): y
	Truncated yourdb.cache_entity_node.
	Truncate yourdb.cache_entity_taxonomy_term? (y/n): 
