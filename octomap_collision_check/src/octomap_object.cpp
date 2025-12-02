#include <octomap_collision_check/octomap_object.h>

namespace octomap {

        // BUG FIX: Replace NULL with nullptr for modern C++ compliance
        OctomapObject::OctomapObject()
         : m_ocTree(nullptr), parent(nullptr)
        {

        }

        OctomapObject::OctomapObject(OcTree* tree)
         : m_ocTree(tree), parent(nullptr)
        {

        }

        OctomapObject::~OctomapObject(){
                if (m_ocTree)
                        delete m_ocTree;

                // delete all children
                for (std::vector<OctomapObject*>::iterator it = children.begin(); it != children.end(); ++it) {
                        delete (*it);
                }
                children.clear();

        }

        void OctomapObject::addChild(OctomapObject* child){
                child->setParent(this);
                children.push_back(child);
        }

        void OctomapObject::init(){
                MotionRange r;
                r.min = r.range = r.current = 0.0;
                motionRanges.resize(numDOF(), r);

        }

        void OctomapObject::move(const std::vector<double>& param){
                unsigned dof = numDOF();
                // BUG FIX: Replace assert with proper error handling to prevent crashes in production
                // Validate parameter and motion range sizes match expected DOF before proceeding
                if(param.size() != dof){
                        std::cerr << "ERROR: Invalid parameter size " << param.size() << ", expected " << dof << std::endl;
                        return;
                }
                if(motionRanges.size() != dof){
                        std::cerr << "ERROR: Invalid motion ranges size " << motionRanges.size() << ", expected " << dof << std::endl;
                        return;
                }

                // TODO check range?
                for (unsigned i = 0; i < dof; ++i){
                        motionRanges[i].current = param[i];
                        motionRanges[i].current = std::min(motionRanges[i].current, 1.0);
                }

                mapParams();
        }

        void OctomapObject::moveDiff(const std::vector<double>& param){
                unsigned dof = numDOF();
                // BUG FIX: Replace assert with proper error handling to prevent crashes in production
                // Validate parameter and motion range sizes match expected DOF before proceeding
                if(param.size() != dof){
                        std::cerr << "ERROR: Invalid parameter size " << param.size() << ", expected " << dof << std::endl;
                        return;
                }
                if(motionRanges.size() != dof){
                        std::cerr << "ERROR: Invalid motion ranges size " << motionRanges.size() << ", expected " << dof << std::endl;
                        return;
                }

                for (unsigned i = 0; i < dof; ++i){
                        motionRanges[i].current += param[i];
                        motionRanges[i].current = std::min(motionRanges[i].current, 1.0);
                }

                mapParams();
        }

        void OctomapObject::setRange(unsigned dofIdx, double min, double max){
                // BUG FIX: Replace assert with proper error handling to prevent crashes in production
                // Validate DOF index is within valid range before accessing motionRanges
                if(dofIdx >= numDOF()){
                        std::cerr << "ERROR: Invalid DOF index " << dofIdx << ", must be less than " << numDOF() << std::endl;
                        return;
                }

                motionRanges.at(dofIdx).min = min;
                motionRanges.at(dofIdx).range = max-min;
        }

        void StaticObject::mapParams(){
                return;
        }

        void PlanarObject::mapParams(){
                origin.trans().x() = mapRange(0);
                origin.trans().y() = mapRange(1);
                double yaw = mapRange(2);
                origin.rot() = octomath::Quaternion(0.0, 0.0, yaw);

        }


        void PrismaticObject::mapParams(){
                origin.trans().x() = mapRange(0);
        }






    
} //namespace
