// -*- tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// vi: set et ts=8 sw=4 sts=4:

#ifndef DUNE_FOAMGRID_EDGE_HH
#define DUNE_FOAMGRID_EDGE_HH

#include <dune/geometry/type.hh>
#include <dune/grid/common/gridenums.hh>

#include "foamgridvertex.hh"


namespace Dune {

    template <int dimworld>
    class FoamGridEntityImp<1,dimworld>
        : public FoamGridEntityBase
    {
    public:



        enum MarkState { DO_NOTHING , COARSEN , REFINE, IS_COARSENED };

        FoamGridEntityImp(const FoamGridEntityImp<0,dimworld>* v0,
                          const FoamGridEntityImp<0,dimworld>* v1,
                          int level, unsigned int id)
            : FoamGridEntityBase(level,id), nSons_(0), father_(nullptr)
        {
            vertex_[0] = v0;
            vertex_[1] = v1;
            sons_[0] =sons_[1] = nullptr;
        }


        FoamGridEntityImp(const FoamGridEntityImp<0,dimworld>* v0,
                          const FoamGridEntityImp<0,dimworld>* v1,
                          int level, unsigned int id,
                          FoamGridEntityImp* father)

            : FoamGridEntityBase(level,id), nSons_(0), father_(father)
        {
            vertex_[0] = v0;
            vertex_[1] = v1;
            sons_[0] =sons_[1] = nullptr;
        }

        /** \todo Implement this method! */
        bool isLeaf() const {
            return sons_[0]==nullptr;
        }


        GeometryType type() const {
            return GeometryType(1);
        }

        /** \brief Number of corners (==2) */
        unsigned int corners() const {
            return 2;
        }

        FieldVector<double, dimworld> corner(int i) const {
            return vertex_[i]->pos_;
        }

        PartitionType partitionType() const {
            return InteriorEntity;
        }

        /** \brief Return level index of sub entity with codim = cc and local number i
         */
        int subLevelIndex (int i, unsigned int codim) const {
            assert(1<=codim && codim<=2);
            switch (codim) {
            case 1:
                return this->levelIndex_;
            case 2:
                return vertex_[i]->levelIndex_;
            }
            DUNE_THROW(GridError, "Non-existing codimension requested!");
        }

        /** \brief Return leaf index of sub entity with codim = cc and local number i
         */
        int subLeafIndex (int i,unsigned int codim) const {
            assert(1<=codim && codim<=2);
            switch (codim) {
            case 1:
                return this->leafIndex_;
            case 2:
                return vertex_[i]->leafIndex_;
            }
            DUNE_THROW(GridError, "Non-existing codimension requested!");
        }

        int refinementIndex_;

	bool isNew_;

	MarkState markState_;


        const FoamGridEntityImp<0,dimworld>* vertex_[2];

        /** \brief links to refinements of this edge */
        array<FoamGridEntityImp<1,dimworld>*,2> sons_;

        /** \brief The number of refined edges (0 or 2). */
        unsigned int nSons_;

        /** \brief Pointer to father element */
        FoamGridEntityImp<1,dimworld>* father_;

    };

}

#endif
