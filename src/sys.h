#ifndef SYS235_H
#define SYS235_H


namespace sys{

	using uint16 = std::uint16_t;

	using Real = float;
	using v3r = aga3::Mv1<Real>;
	using s1r = aga3::Mv0<Real>;

	using PartId = uint16;
	using LinkId = uint16;

	BOOST_FUSION_DEFINE_STRUCT_INLINE(
		Link,
		(PartId, i)
		(PartId, j)
		(Real, c)
		(Real, k)
		(Real, l)
	)


	BOOST_FUSION_DEFINE_STRUCT_INLINE(
		Part,
		(v3r, p)
		(v3r, q)
		(Real, m)
	)


	struct Sys{
		std::vector<Part> parts;
		std::vector<Link> links;

		PartId create_part(v3r const& pos) {
			parts.push_back(Part(pos, {0,0,0}, 1));
			return parts.size() - 1;
		}


		LinkId create_link(PartId i, PartId j, Real l) {
			links.push_back({i, j, Real(0.5), Real(0.5), l});
			return links.size() - 1;
		}

		LinkId create_link(PartId i, PartId j) {
			auto pi = parts.at(i).p;
			auto pj = parts.at(j).p;
			Real l = sqrt((pi - pj)|(pi - pj));
			return create_link(i, j, l);
		}



		PartId get_num_part() {
			return parts.size();
		}

		LinkId get_num_link() {
			return links.size();
		}

	};



	 std::vector<v3r> & calc_forces(std::vector<v3r> & fs, Sys const& sys) {

		auto& parts = sys.parts;
		auto& links = sys.links;

		fs.assign(parts.size(), {0,0,0});

		for (auto& link: links) {
			auto i = link.i;
			auto j = link.j;

			auto lij = link.l;

			auto const& part_i = parts.at(link.i);
			auto const& part_j = parts.at(link.j);

			auto const& pi = part_i.p;
			auto const& pj = part_j.p;

			auto const& qi = part_i.q;
			auto const& qj = part_j.q;

			auto kij = link.k;
			auto cij = link.c;

			auto dp = pi - pj;
			auto d2 = aga3::inn2(dp);
			auto d = sqrt(d2);

			Real spring = (kij * (1 - lij/d));
			Real dump_i = (cij * (qi|dp) / d2);
			Real dump_j = (cij * (qj|dp) / d2);

			fs.at(i) += (-spring - dump_i) * dp;
			fs.at(j) += (+spring - dump_j) * dp;
		}

		return fs;
	}
}


#endif
