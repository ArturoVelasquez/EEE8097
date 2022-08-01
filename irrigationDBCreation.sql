DROP TABLE public.node_types cascade;

CREATE TABLE public.node_types (
	id serial4 NOT NULL,
	type_name varchar NOT NULL,
	description text NOT NULL,
	active bool NOT NULL DEFAULT true,
	CONSTRAINT node_types_pkey PRIMARY KEY (id)
);

DROP TABLE public.nodes;

CREATE TABLE public.nodes (
	id serial4 NOT NULL,
	node_name varchar NULL,
	mobile bool NOT NULL DEFAULT false,
	latitude float4 NULL,
	longitude float4 NULL,
	type_id int4 NULL,
	active bool NOT NULL DEFAULT true,
	inactivation_date timestamp NULL,
	CONSTRAINT nodes_pkey PRIMARY KEY (id),
	CONSTRAINT node_types_fkey FOREIGN KEY (type_id) REFERENCES public.node_types(id)
);

DROP TABLE public.measurements;

CREATE TABLE public.measurements (
	id serial4 NOT NULL,
	node_id int4 NOT NULL,
	time_taken timestamp NOT NULL DEFAULT now(),
	temperature float4 NULL,
	humidity float4 NULL,
	soil_moisture float4 NULL,
	light_intensity float4 NULL,
	sodium float4 NULL,
	potasium float4 NULL,
	phosphorus float4 NULL,
	gps_location bool NOT NULL DEFAULT false,
	latitude float4 NULL,
	longitude float4 NULL,
	CONSTRAINT measurements_pkey PRIMARY KEY (id),
	CONSTRAINT measurements_nodes_fkey FOREIGN KEY (node_id) REFERENCES public.nodes(id)
);
