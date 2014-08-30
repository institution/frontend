#ifndef SYS235_H
#define SYS235_H


namespace sys{

	using uint16 = std::uint16_t;

	using Real = float;
	using v3r = aga3::Mv1<Real>;

	using PartId = uint16;
	using LinkId = uint16;

	BOOST_FUSION_DEFINE_STRUCT_INLINE(
		Link,
		(PartId, i)
		(PartId, j)
		(Real, c)
		(Real, k)
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

		LinkId create_link(PartId i, PartId j) {
			links.push_back({i,j, 0.5, 0.5});
			return links.size() - 1;
		}

		PartId get_num_part() {
			return parts.size();
		}

		LinkId get_num_link() {
			return links.size();
		}

	};



	vector<v3r> calc_forces(Sys const& sys) {

		auto& parts = sys.parts;
		auto& links = sys.links;

		vector<v3r> fs;
		fs.assign(parts.size(), {0,0,0});

		for (auto& link: links) {
			auto i = link.i;

			auto const& part_i = parts.at(link.i);
			auto const& part_j = parts.at(link.j);

			auto const& pi = part_i.p;
			auto const& pj = part_j.p;

			auto const& qi = part_i.q;

			auto kij = link.k;
			auto cij = link.c;

			auto dp = pi - pj;
			auto d2 = dp * dp;
			auto d = sqrt(dp2);

			auto mm = (kij * (1 - lij/d)) + (cij * (qi|dp) / d2);
			auto f = - mm * dp;

			fs.at(i) += f;
		}

		return fs;
	}
}


#endif
