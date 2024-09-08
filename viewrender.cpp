#include "includes.h"

void Hooks::OnRenderStart( ) {
	// call og.
	g_hooks.m_view_render.GetOldMethod< OnRenderStart_t >( CViewRender::ONRENDERSTART )( this );

	if( g_menu.main.visuals.fov.get( ) ) {
		if( g_cl.m_local && g_cl.m_local->m_bIsScoped( ) ) {
			if( g_menu.main.visuals.fov_scoped.get( ) ) {
				if( g_cl.m_local->GetActiveWeapon( )->m_zoomLevel( ) != 2 ) {
					g_csgo.m_view_render->m_view.m_fov = g_menu.main.visuals.fov_amt.get( );
				}
				else {
					g_csgo.m_view_render->m_view.m_fov += 45.f;
				}
			}
		}

		else g_csgo.m_view_render->m_view.m_fov = g_menu.main.visuals.fov_amt.get( );
	}

	if( g_menu.main.visuals.viewmodel_fov.get( ) )
		g_csgo.m_view_render->m_view.m_viewmodel_fov = g_menu.main.visuals.viewmodel_fov_amt.get( );
}

void MotionBlur()
{
	if (!g_csgo.m_cvar)
		return;

	int value = g_menu.main.visuals.motion_blur.get();
	static ConVar* mat_motion_blur_enabled = g_csgo.m_cvar->FindVar(HASH("mat_motion_blur_enabled"));
	static ConVar* mat_motion_blur_strength = g_csgo.m_cvar->FindVar(HASH("mat_motion_blur_strength"));

	if (value > 0) {
		mat_motion_blur_enabled->SetValue(1);
		mat_motion_blur_strength->SetValue(value);
	}
	else {
		mat_motion_blur_enabled->SetValue(0);
		mat_motion_blur_strength->SetValue(0);
	}
}

void Hooks::RenderView( const CViewSetup &view, const CViewSetup &hud_view, int clear_flags, int what_to_draw ) {
	// ...

	MotionBlur();

	if (g_csgo.viewmodel_offset_x)
		g_csgo.viewmodel_offset_x->SetValue(g_menu.main.visuals.viewmodel_x.get() / 2.f);
	if (g_csgo.viewmodel_offset_y)
		g_csgo.viewmodel_offset_y->SetValue(g_menu.main.visuals.viewmodel_y.get() / 2.f);
	if (g_csgo.viewmodel_offset_z)
		g_csgo.viewmodel_offset_z->SetValue(g_menu.main.visuals.viewmodel_z.get() / 2.f);

	g_hooks.m_view_render.GetOldMethod< RenderView_t >( CViewRender::RENDERVIEW )( this, view, hud_view, clear_flags, what_to_draw );
}

void Hooks::Render2DEffectsPostHUD( const CViewSetup &setup ) {
	if( !g_menu.main.visuals.noflash.get( ) )
		g_hooks.m_view_render.GetOldMethod< Render2DEffectsPostHUD_t >( CViewRender::RENDER2DEFFECTSPOSTHUD )( this, setup );
}

void Hooks::RenderSmokeOverlay( bool unk ) {
	// do not render smoke overlay.
	if( !g_menu.main.visuals.nosmoke.get( ) )
		g_hooks.m_view_render.GetOldMethod< RenderSmokeOverlay_t >( CViewRender::RENDERSMOKEOVERLAY )( this, unk );
}
