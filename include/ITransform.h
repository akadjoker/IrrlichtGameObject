#ifndef __I_TRANSFORM_H_INCLUDED
#define __I_TRANSFORM_H_INCLUDED

#include "IReferenceCounted.h"


#include "SMaterial.h"
#include "irrString.h"
#include "aabbox3d.h"
#include "vector3d.h"
#include "vector2d.h"
#include "quaternion.h"
#include "matrix4.h"
#include "irrList.h"


namespace irr
{
namespace scene
{

class ITransform : public virtual IReferenceCounted
{
public:

        ITransform();
        ITransform(scene::ITransform* parent);
        ITransform(scene::ITransform* parent,const core::vector3df& position);
        ITransform(scene::ITransform* parent,
                const core::vector3df& position,
				const core::vector3df& rotation,
				const core::vector3df& scale);


    virtual ~ITransform(){  };



      //! Gets the scale of the scene node relative to its parent.
		/** This is the scale of this node relative to its parent.
		If you want the absolute scale, use
		getAbsoluteTransformation().getScale()
		\return The scale of the scene node. */
		 const core::vector3df& getScale() const
		{
			return RelativeScale;
		}


		//! Sets the relative scale of the scene node.
		/** \param scale New scale of the node, relative to its parent. */
		void setScale(const core::vector3df& scale)
		{
			RelativeScale = scale;
		}

				//! Sets the relative scale of the scene node.
		/** \param scale New scale of the node, relative to its parent. */
		void setScale(float x, float y,float z)
		{
			setScale(core::vector3df(x,y,z));
		}

        //! Sets the relative scale of the scene node.
		/** \param scale New scale of the node, relative to its parent. */
		void setScale(float scale)
		{
			setScale(core::vector3df(scale,scale,scale));
		}

		//! Gets the rotation of the node relative to its parent.
		/** Note that this is the relative rotation of the node.
		If you want the absolute rotation, use
		getAbsoluteTransformation().getRotation()
		\return Current relative rotation of the scene node. */
		 const core::vector3df& getRotation() const
		{
			return RelativeRotation;
		}


		//! Sets the rotation of the node relative to its parent.
		/** This only modifies the relative rotation of the node.
		\param rotation New rotation of the node in degrees. */
		ITransform& setRotation(const core::vector3df& rotation)
		{
			RelativeRotation = rotation;
			return (*this);
		}

		ITransform& setRotation(float x, float y, float z)
		{
			RelativeRotation = core::vector3df(x,y,z);
			return (*this);
		}

		ITransform& setRotation(core::vector3df axis,float angle)
		{
			 core::quaternion quat;
			 quat.fromAngleAxis ( angle, axis).toEuler(Rotation);
			 RelativeRotation+=Rotation;

			return (*this);
		}
		//! Gets the position of the node relative to its parent.
		/** Note that the position is relative to the parent. If you want
		the position in world coordinates, use getAbsolutePosition() instead.
		\return The current position of the node relative to the parent. */
		 const core::vector3df& getPosition() const
		{
			return RelativeTranslation;
		}


		//! Sets the position of the node relative to its parent.
		/** Note that the position is relative to the parent.
		\param newpos New relative position of the scene node. */
		ITransform& setPosition(const core::vector3df& newpos)
		{
			RelativeTranslation = newpos;
			return (*this);
		}

		ITransform& setPosition(float x,float y,float z)
		{
			RelativeTranslation = core::vector3df(x,y,z);
			return (*this);
		}


		//! Gets the absolute position of the node in world coordinates.
		/** If you want the position of the node relative to its parent,
		use getPosition() instead.
		NOTE: For speed reasons the absolute position is not
		automatically recalculated on each change of the relative
		position or by a position change of an parent. Instead the
		update usually happens once per frame in OnAnimate. You can enforce
		an update with updateAbsolutePosition().
		\return The current absolute position of the scene node (updated on last call of updateAbsolutePosition). */
		 core::vector3df getAbsolutePosition() const
		{
			return AbsoluteTransformation.getTranslation();
		}

			//! Changes the parent of the scene node.
		/** \param newParent The new parent to be used. */
		void setParent(ITransform* newParent)
		{
			Parent = newParent;
		}
         //! Updates the absolute position based on the relative and the parents position
		/** Note: This does not recursively update the parents absolute positions, so if you have a deeper
			hierarchy you might want to update the parents first.*/
		void updateAbsolutePosition()
		{
			if (Parent)
			{
				AbsoluteTransformation =			Parent->getAbsoluteTransformation() * getRelativeTransformation();
			}
			else
				AbsoluteTransformation = getRelativeTransformation();
		}


		//! Returns the parent of this scene node
		/** \return A pointer to the parent. */
		scene::ITransform* getParent() const
		{
			return Parent;
		}
        void OnUpdate(u32 timeMs)
		{
				updateAbsolutePosition();
         }

	//! Get the absolute transformation of the node. Is recalculated every OnAnimate()-call.
		/** NOTE: For speed reasons the absolute transformation is not
		automatically recalculated on each change of the relative
		transformation or by a transformation change of an parent. Instead the
		update usually happens once per frame in OnAnimate. You can enforce
		an update with updateAbsolutePosition().
		\return The absolute transformation matrix. */
		virtual const core::matrix4& getAbsoluteTransformation() const
		{
			return AbsoluteTransformation;
		}


		//! Returns the relative transformation of the scene node.
		/** The relative transformation is stored internally as 3
		vectors: translation, rotation and scale. To get the relative
		transformation matrix, it is calculated from these values.
		\return The relative transformation matrix. */
		virtual core::matrix4 getRelativeTransformation() const
		{
			core::matrix4 mat;
			mat.setRotationDegrees(RelativeRotation);
			mat.setTranslation(RelativeTranslation);

			if (RelativeScale != core::vector3df(1.f,1.f,1.f))
			{
				core::matrix4 smat;
				smat.setScale(RelativeScale);
				mat *= smat;
			}

			return mat;
		}

private:

protected:

//! Absolute transformation of the node.
core::matrix4 AbsoluteTransformation;

//! Relative translation of the scene node.
core::vector3df RelativeTranslation;

//! Relative rotation of the scene node.
core::vector3df RelativeRotation;
core::vector3df Rotation;

//! Relative scale of the scene node.
core::vector3df RelativeScale;

//! Pointer to the parent
ITransform* Parent;
};


} // end namespace video
} // end namespace irr

#endif
