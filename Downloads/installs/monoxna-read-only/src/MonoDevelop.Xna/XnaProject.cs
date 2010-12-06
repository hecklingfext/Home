#region License
/*
MIT License
Copyright © 2009 The Mono.Xna Team

All rights reserved.

Authors:
Lars Magnusson (lavima)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#endregion License

using System;
using System.Xml;
using System.IO;
using MonoDevelop.Core;
using MonoDevelop.Core.Serialization;
using MonoDevelop.Projects;
using MonoDevelop.Projects.Dom;
using MonoDevelop.Core.ProgressMonitoring;


namespace MonoDevelop.Xna
{

	public class XnaProject : DotNetProject
    {
		#region Fields
		
		[ItemProperty("XnaFrameworkVersion")]
		protected string xnaFrameworkVersion = "v2.0";
		
		#endregion Fields
		
		#region Properties
		
		[ItemProperty("XnaPlatform")]
		protected XnaPlatformType xnaPlatform;
		public XnaPlatformType XnaPlatform {
			get { return xnaPlatform; }
			set { xnaPlatform = value; }
		}
		
		protected NestedContentProjectCollection nestedContentProjects;
		public NestedContentProjectCollection NestedContentProjects {
			get { return nestedContentProjects; }	
		}
		
		#endregion Properties

        #region Constructors
		
		public XnaProject ()
			: this ("C#") {}
		
		public XnaProject (string languageName)
			: base (languageName)
		{
			nestedContentProjects = new NestedContentProjectCollection(this);			
			Items.Bind(nestedContentProjects);
		}
		
		public XnaProject (string languageName, ProjectCreateInformation info, XmlElement projectOptions)
			: base (languageName, info, projectOptions)
		{
			nestedContentProjects = new NestedContentProjectCollection(this);			
			Items.Bind(nestedContentProjects);		
		}

        #endregion Constructors

		#region Public Methods
		
		public void AddContentProject (ContentProject contentProject)
		{
			RegisterInternalChild(contentProject);	
		}
		
		#endregion Public Methods

        #region DotNetProject Overrides
		
		public override string ProjectType {
			get  { return "Xna"; }
		}
				
		protected override BuildResult DoBuild (IProgressMonitor monitor, ConfigurationSelector configuration)
		{
			foreach(NestedContentProject nestedProj in nestedContentProjects)
				nestedProj.Project.Build(monitor, configuration);
			
            return base.DoBuild(monitor, configuration); 
		}		
		
		public override void Save (IProgressMonitor monitor)
		{
			foreach(NestedContentProject nested in nestedContentProjects)
			{
				FilePath nestedPath = this.BaseDirectory.Combine(nested.Include);
				if(!Directory.Exists(nestedPath.ParentDirectory))
					Directory.CreateDirectory(nestedPath.ParentDirectory);
				nested.Project.Save(nestedPath, monitor);
			}
			
			base.Save (monitor);
		}
		
		protected override void OnEndLoad ()
		{
			foreach(NestedContentProject nestedProject in nestedContentProjects)
				nestedProject.Parent = this;
		}
		
		protected override void OnConfigurationAdded (ConfigurationEventArgs args)
		{
			base.OnConfigurationAdded (args);
		}
		
		protected override void OnConfigurationRemoved (ConfigurationEventArgs args)
		{
			base.OnConfigurationRemoved (args);
		}

        #endregion DotNetProject Overrides
		
    }

}
