# Learning Log

A running record of what I did, what broke, how I fixed it, and what I learned.
Newest entries at the top. Keep each cell short; link out for details.

| time | task | problem | fix | insight |
|------|------|---------|-----|---------|
| 2024-06-12 10:15 | Set up local dev env for `api-service` | `docker compose up` failed: port 5432 already in use | `lsof -i :5432` → stopped local Postgres; re-ran compose | Always check for a host-bound DB before blaming the container config |